from functools import wraps
from NotificationService.App.Repositories.repository import NotificationRepository
from copy import deepcopy


class UnitOfWorkNotification:
    def __init__(self, repository: NotificationRepository):
        self.repository_main = repository

    def __enter__(self):
        self.repository = deepcopy(self.repository_main)
        return self

    def __exit__(self, exc_type, exc_value, traceback) -> bool:
        if exc_type:
            self.rollback()
        else:
            self.commit()
        return False

    def commit(self):
        self.repository_main.notifications = self.repository.notifications

    def rollback(self):
        pass


def unit_of_work(func):
    @wraps(func)
    def wrapper(self, repository, *args, **kwargs):
        with UnitOfWorkNotification(repository) as uow:
            return func(self, uow, *args, **kwargs)
    return wrapper
