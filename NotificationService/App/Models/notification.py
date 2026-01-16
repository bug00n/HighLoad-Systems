from enum import Enum
from dataclasses import dataclass
from NotificationService.exceptions import UserNotFound, NotificationNotFound, InvalidNotificationState


class Status(Enum):
    NEW = "new"
    READ = "read"
    DELETED = "deleted"


@dataclass(frozen=True)
class Notification:
    id: int
    message: str


@dataclass
class Notify:
    notification: Notification
    status: Status
