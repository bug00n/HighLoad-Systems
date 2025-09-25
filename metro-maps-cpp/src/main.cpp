#include <queue>
#include <thread>
#include <string>
#include <iostream>
#include <libpq-fe.h>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <hiredis/async.h>
#include <hiredis/hiredis.h>


// CONF DATA
constexpr int16_t PORT = 8000;
const char* const IP = "0.0.0.0"; // Open for every web-interfaces;


// DATA BASE CREDENTIALS
constexpr int16_t PORT_ = 5432;
const char* const HOST_ = "127.0.0.1"; // Only localhost
const char* const DBNAME_ = "database-maps";
const char* const USER_ = "admin";
const char* const PASSWORD_ = "p@ssw0rd";


// CACHE CREDENTIALS
constexpr int16_t PORT__ = 6379;
const char* const HOST__ = "127.0.0.1"; // Only localhost


using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;


class Redis : public std::enable_shared_from_this <Redis>  {
  private:
    

  public:
    
};

class PostgreSQL : public std::enable_shared_from_this <PostgreSQL> {
  private:
    boost::asio::io_context& io_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::steady_timer timer_;
    PGconn* conn_ = nullptr;
    bool conn_ready_ = false;
    std::queue <std::string> pending_queries_;

    void start_conn(const std::string& conninfo) {
      conn_ = PQconnectStart(conninfo.c_str());

      if (conn_ == nullptr || PQstatus(conn_) == CONNECTION_BAD) {
        std::cerr << "Connection start failed: " << PQerrorMessage(conn_) << std::endl;
        reconn_aft_del();
        return;
      }
        
      setup_async_connection();
    }

    void setup_async_connection() {
      int socket_id = PQsocket(conn_);
      if (socket_id < 0) {
        std::cerr << "Invalid socket" << std::endl;
        reconn_aft_del();
        return;
      }
      
      socket_.assign(boost::asio::ip::tcp::v4(), socket_id);
      // poll_connection_status();
    }

    void poll_connection_status() {
        PostgresPollingStatusType status = PQconnectPoll(conn_);
        
        if (status == PGRES_POLLING_OK) {
          on_conn_established();
        }
        if (status == PGRES_POLLING_READING) {
          socket_.async_wait(boost::asio::ip::tcp::socket::wait_read,
            [this] (boost::system::error_code ec)
          {
            if (!ec) poll_connection_status();
            else handle_connection_error();
          });
        }
        if (status == PGRES_POLLING_WRITING) {
          socket_.async_wait(boost::asio::ip::tcp::socket::wait_write,
            [this] (boost::system::error_code ec)
          {
            if (!ec) poll_connection_status();
            else handle_connection_error();
          });
        }
        handle_connection_error();
    }

    void on_conn_established() {
      std::cout << "Connected to DataBase" << std::endl;
      conn_ready_ = true;
      
      while (!pending_queries_.empty()) {
        send_query(pending_queries_.front());
        pending_queries_.pop();
      }
    }

    void send_query(const std::string& query) {
      if (PQsendQuery(conn_, query.c_str()) != 1) {
        std::cerr << "Query failed: " << PQerrorMessage(conn_) << std::endl;
        return;
      }
      
      wait_for_query_result();
    }

    void wait_for_query_result() {
      socket_.async_wait(boost::asio::ip::tcp::socket::wait_read,
        [this](boost::system::error_code ec)
      {
        if (ec) {
          handle_connection_error();
          return;
        }
        
        PQconsumeInput(conn_);
        if (PQisBusy(conn_)) {
          wait_for_query_result();
        } else {
          process_query_result();
        }
      });
    }

    void process_query_result() {
      PGresult* result = PQgetResult(conn_);
        
      if (result) {
        if (PQresultStatus(result) == PGRES_TUPLES_OK) {
          int rows = PQntuples(result);
          int cols = PQnfields(result);
          
          std::cout << "Query result: " << rows << " rows, " << cols << " columns" << std::endl;
          
          for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
              std::cout << PQgetvalue(result, i, j) << " ";
            }
            std::cout << std::endl;
          }
        }
        
        PQclear(result);
        
        if (PQgetResult(conn_) != nullptr) {
          process_query_result();
        } 
      }
    }

    void handle_connection_error() {
      std::cerr << "Connection error: " << PQerrorMessage(conn_) << std::endl;
      cleanup_connection();
      reconn_aft_del();
    }

    void cleanup_connection() {
      if (conn_) {
        PQfinish(conn_);
        conn_ = nullptr;
      }
      conn_ready_ = false;
      
      if (socket_.is_open()) {
        boost::system::error_code ec;
        socket_.close(ec);
      }
    }

    void reconn_aft_del() {
      timer_.expires_after(std::chrono::seconds(5));
      timer_.async_wait([this] (boost::system::error_code ec) {
        if (!ec) {
          std::cout << "Attempting to reconnect..." << std::endl;
          connect();
        }
      });
    }

  public:
    explicit PostgreSQL(boost::asio::io_context& io) 
      : io_(io), socket_(io), timer_(io)
    { }

    void connect() {
        // start_connection(conninfo);
    }
};

class DataPackage {
  public:
    std::string name_from;
    std::string name_to;

    int32_t dist; // in meters
    int32_t time; // in seconds
};

class DataHandler {
  private:

  public:
    explicit DataHandler() {}

    
};

class Connection : public std::enable_shared_from_this <Connection> {
  private:
    tcp::socket socket;
    beast::flat_buffer buffer;
    http::request <http::string_body> request;
    http::response <http::string_body> response;

    void read_req() {
      auto self = shared_from_this();
      http::async_read(socket, buffer, request,
        [self] (beast::error_code ec, auto /*bytes*/)
      {
        if (!ec) {
          self->handle_req();
        }
      });
    }

    void handle_req() {
      response.version(request.version());
      response.keep_alive(false);

      if (request.method() == http::verb::get) {
        handle_get();
      } else {
        response.result(http::status::bad_request);
        response.set(http::field::content_type, "text/plain");
        response.body() = "Invalid method";
      }
      
      write_response();
    }

    void handle_get() {
      std::string target(request.target());
      
      if (target == "/main") {
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/html");
        response.body() = "Hello from server!<br>Use it!<br><a href=\"http://127.0.0.1:8000/maps\">Click</a>";
      } else if (target == "/maps") {
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/html");
        response.body() = "There is nothing(((";
      } else if (target == "/maps/mstusfl") {
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/html");
        response.body() = "Well, there is the most used routes<br>1...";
      } else {
        response.result(http::status::not_found);
        response.set(http::field::content_type, "text/html");
        response.body() = "404 Not Found";
      }
    }

    void write_response() {
        auto self = shared_from_this();
        
        response.set(http::field::server, "Boost.Asio HTTP Server");
        response.set(http::field::content_length, std::to_string(response.body().size()));
        
        http::async_write(socket, response,
            [self](beast::error_code ec, auto)
        {
            self->socket.shutdown(tcp::socket::shutdown_send, ec);
        });
    }

  public:
    explicit Connection(tcp::socket socket_) : socket(std::move(socket_)) { }

    void run() {
      read_req();
    }
};

class Server {
 private:
    boost::asio::io_context& io;
    tcp::acceptor acceptor;

    void accept_conn() {
      acceptor.async_accept(
        [this] (beast::error_code ec, tcp::socket socket)
      {
        if (!ec) {
          // A new iterator creates inside the recursion
          std::make_shared <Connection> (std::move(socket))->run();
        }
        accept_conn();
      });
    }

 public:
    explicit Server(boost::asio::io_context& io_, const int16_t port_)
      : io(io_), acceptor(io_, tcp::endpoint(tcp::v4(), port_))
    {
      std::cout << "The server was started" << std::endl;
      accept_conn();
    }       
};


int main(int argc, char **argv) {
  try {
    boost::asio::io_context io;

    // correct ctrl + C
    boost::asio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait([&](auto /*ex*/, auto /*sig*/)
    {
      std::cout << "\tThe server has been stopped" << std::endl;
      io.stop();
    });

    Server server(io, PORT);

    size_t thread_count = std::thread::hardware_concurrency();
    std::vector <std::thread> server_handlers;

    // devide by 2 because I need some threads more
    // other threads should be used for algorithms part of server

    server_handlers.reserve(thread_count / 2); 
    for (size_t i = 0; i < thread_count / 2; ++i) {
        server_handlers.emplace_back([&io]() {
            io.run();
        });
    }

    for (std::thread &th : server_handlers) {
      th.join();
    }

    return EXIT_SUCCESS;
  } catch (std::exception& error) {
    std::cerr << error.what() << std::endl; 
    return EXIT_FAILURE;
  }
}