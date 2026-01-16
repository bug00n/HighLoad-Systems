from NotificationService.App.Models.notification import Notification, Notify, Status
from NotificationService.exceptions import InvalidNotificationState
from NotificationService.App.UoW.notification_uow import unit_of_work, UnitOfWorkNotification
from NotificationService.App.Repositories.repository import NotificationRepository


class NotificationManager:
    def _check_status(self, notify: Notify, expected_status: list):
        if notify.status not in expected_status:
            raise InvalidNotificationState(f"Notification status is {notify.status}, expected one of {[s.value for s in expected_status]}")

    @unit_of_work
    def add_notification(self, uow: UnitOfWorkNotification, user_id: int, notification: Notification):
        uow.repository.add(user_id, notification) 
        
    @unit_of_work
    def get_new_notifications(self, uow: UnitOfWorkNotification, user_id: int) -> list:
        return [notify.notification for notify in uow.repository.get_all(user_id) if notify.status == Status.NEW]

    @unit_of_work
    def mark_read(self, uow: UnitOfWorkNotification, user_id: int, notification_id: int):
        notify = uow.repository.get(user_id, notification_id)
        self._check_status(notify, [Status.NEW])
        uow.repository.update(user_id, notification_id, Status.READ)

    @unit_of_work
    def mark_all_read(self, uow: UnitOfWorkNotification, user_id: int):
        for notify in uow.repository.get_all(user_id):
            if notify.status == Status.NEW:
                uow.repository.update(user_id, notify.notification.id, Status.READ)
            
    @unit_of_work
    def delete_notification(self, uow: UnitOfWorkNotification, user_id: int, notification_id: int):
        notify = uow.repository.get(user_id, notification_id)
        self._check_status(notify, [Status.NEW, Status.READ])
        uow.repository.update(user_id, notification_id, Status.DELETED)

    @unit_of_work
    def get_user_notifications(self, uow: UnitOfWorkNotification, user_id: int) -> list:
        return [notify.notification for notify in uow.repository.get_all(user_id) if notify.status != Status.DELETED]

    @unit_of_work
    def get_notification(self, uow: UnitOfWorkNotification, user_id: int, notification_id: int) -> Notification:
        notify = uow.repository.get(user_id, notification_id)
        self._check_status(notify, [Status.NEW, Status.READ])
        return notify.notification
