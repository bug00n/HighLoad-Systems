from NotificationService.App.Models.notification import Notification, Status, Notify
from NotificationService.exceptions import NotificationConflict, UserNotFound, NotificationNotFound


class NotificationRepository:
    def __init__(self):
        self.notifications = {}

    def _get_user_notifications(self, user_id: int) -> dict:
        if user_id not in self.notifications:
            raise UserNotFound(f"User {user_id} not found")
        return self.notifications[user_id]

    def _get_notification(self, user_id: int, notification_id: int) -> Notify:
        user_notifications = self._get_user_notifications(user_id)

        if notification_id not in user_notifications:
            raise NotificationNotFound("Notification not found")
        
        return user_notifications[notification_id]

    def add(self, user_id: int, notification: Notification):
        if user_id not in self.notifications:
            self.notifications[user_id] = {}

        if notification.id in self.notifications[user_id]:
            raise NotificationConflict(f"Notification with id: {notification.id} is already exist")
        
        self.notifications[user_id][notification.id] = Notify(notification, Status.NEW)

    def get(self, user_id: int, notification_id: int) -> Notify:
        return self._get_notification(user_id, notification_id)
    
    def get_all(self, user_id: int) -> list[Notify]:
        return self._get_user_notifications(user_id).values()
    
    def update(self, user_id: int, notification_id: int, status: Status):
        notify = self._get_notification(user_id, notification_id)
        notify.status = status

    def exists(self, user_id: int, notification_id: int) -> bool:
        if user_id not in self.notifications:
            raise UserNotFound(f"User {user_id} not found")
        return notification_id in self.notifications[user_id]
