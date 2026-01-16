import pytest
from NotificationService.App.Repositories.repository import NotificationRepository
from NotificationService.App.Models.notification import Notification, Notify, Status
from NotificationService.exceptions import UserNotFound, NotificationNotFound, NotificationConflict


# Happy path tests
def test_add_notification_happy_path():
    repository = NotificationRepository()
    notification = Notification(1, "Test Message")
    repository.add(1, notification)

    notify = repository.get(1, 1)

    assert notify.notification == notification
    assert notify.status == Status.NEW

def test_get_all_happy_path():
    repository = NotificationRepository()
    notification1 = Notification(1, "Test Message 1")
    notification2 = Notification(2, "Test Message 2")
    repository.add(1, notification1)
    repository.add(1, notification2)

    notifies = repository.get_all(1)

    assert len(notifies) == 2
    assert {notify.notification for notify in notifies} == {notification1, notification2}

def test_update_notification_happy_path():
    repository = NotificationRepository()
    notification1 = Notification(1, "Test Message")
    notification2 = Notification(2, "Test Message")
    repository.add(1, notification1)
    repository.add(1, notification2)

    repository.update(1, 1, Status.READ)
    repository.update(1, 2, Status.DELETED)

    notify1 = repository.get(1, 1)
    notify2 = repository.get(1, 2)

    assert notify1.status == Status.READ
    assert notify2.status == Status.DELETED

def test_get_notification_happy_path():
    repository = NotificationRepository()
    notification = Notification(1, "Test Message")
    repository.add(1, notification)

    notify = repository.get(1, 1)

    assert notify.notification == notification
    assert notify.status == Status.NEW

def test_exists_happy_path():
    repository = NotificationRepository()
    notification = Notification(1, "Test Message")

    repository.add(1, notification)

    assert repository.exists(1, 1) is True
    assert repository.exists(1, 2) is False


# Edge case tests
def test_get_all_user_not_found():
    repository = NotificationRepository()

    with pytest.raises(UserNotFound):
        repository.get_all(999)

def test_get_notification_not_found():
    repository = NotificationRepository()
    notification = Notification(1, "Test Message")
    repository.add(1, notification)

    with pytest.raises(NotificationNotFound):
        repository.get(1, 999)

def test_update_notification_not_found():
    repository = NotificationRepository()
    notification = Notification(1, "Test Message")
    repository.add(1, notification)

    with pytest.raises(NotificationNotFound):
        repository.update(1, 999, Status.READ)

def test_add_notification_conflict():
    repository = NotificationRepository()
    notification = Notification(1, "Test Message")
    repository.add(1, notification)

    with pytest.raises(NotificationConflict):
        repository.add(1, notification)

def test_get_notification_user_not_found():
    repository = NotificationRepository()

    with pytest.raises(UserNotFound):
        repository.get(1, 1)

def test_update_notification_user_not_found():
    repository = NotificationRepository()

    with pytest.raises(UserNotFound):
        repository.update(1, 1, Status.READ)

def test_exists_user_not_found():
    repository = NotificationRepository()

    with pytest.raises(UserNotFound):
        repository.exists(1, 1)
