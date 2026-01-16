from fastapi import FastAPI, HTTPException
from NotificationService.App.Services.service import NotificationManager
from NotificationService.App.Models.notification import Notification
from NotificationService.exceptions import UserNotFound, NotificationNotFound, InvalidNotificationState
from NotificationService.App.Repositories.repository import NotificationRepository


app = FastAPI()


manager = NotificationManager()
repository = NotificationRepository()


@app.post("/users/{user_id}/notifications")
def add_notification(user_id: int, message: str):
    try:
        notification_id = len(repository.get_all(user_id)) + 1
        notification = Notification(notification_id, message)
        manager.add_notification(repository, user_id, notification)
        return {"status": "ok", "id": notification.id}
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.get("/users/{user_id}/notifications/new")
def get_new_notifications(user_id: int):
    try:
        notifications = manager.get_new_notifications(repository, user_id)
        return [{"id": n.id, "message": n.message} for n in notifications]
    except UserNotFound:
        raise HTTPException(status_code=404, detail="User not found")

@app.put("/users/{user_id}/notifications/{notification_id}/read")
def mark_read(user_id: int, notification_id: int):
    try:
        manager.mark_read(repository, user_id, notification_id)
        return {"status": "ok"}
    except (UserNotFound, NotificationNotFound, InvalidNotificationState) as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.delete("/users/{user_id}/notifications/{notification_id}")
def delete_notification(user_id: int, notification_id: int):
    try:
        manager.delete_notification(repository, user_id, notification_id)
        return {"status": "ok"}
    except (UserNotFound, NotificationNotFound, InvalidNotificationState) as e:
        raise HTTPException(status_code=400, detail=str(e))
