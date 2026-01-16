import pytest
from NotificationService.App.Repositories.repository import NotificationRepository
from NotificationService.App.Services.service import NotificationManager
from NotificationService.App.Models.notification import Notification
from NotificationService.exceptions import UserNotFound, NotificationNotFound, InvalidNotificationState
from NotificationService.App.UoW.notification_uow import UnitOfWorkNotification as UnitOfWork


@pytest.fixture
def repository():
    return NotificationRepository()

@pytest.fixture
def notification_system():
    return NotificationManager()


# Happy path tests
def test_add_notification_happy_path(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message"))
    notification_system.add_notification(repository, 1, Notification(2, "Test Message"))

    new_notifications = notification_system.get_new_notifications(repository, 1)

    assert set(new_notifications) == {Notification(1, "Test Message"), Notification(2, "Test Message")}

def test_mark_read_happy_path(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message"))
    notification_system.add_notification(repository, 1, Notification(2, "Test Message"))
    notification_system.mark_read(repository, 1, 1)

    new_notifications = notification_system.get_new_notifications(repository, 1)

    assert set(new_notifications) == {Notification(2, "Test Message")}

    all_notifications = notification_system.get_user_notifications(repository, 1)

    assert set(all_notifications) == {Notification(1, "Test Message"), Notification(2, "Test Message")}

def test_delete_notification_happy_path(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message"))
    notification_system.add_notification(repository, 1, Notification(2, "Test Message"))
    notification_system.delete_notification(repository, 1, 1)

    user_notifications = notification_system.get_user_notifications(repository, 1)

    assert set(user_notifications) == {Notification(2, "Test Message")}

def test_get_notification_happy_path(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message"))
    notification_system.add_notification(repository, 1, Notification(2, "Test Message"))

    notification = notification_system.get_notification(repository, 1, 1)

    assert notification == Notification(1, "Test Message")

    notification = notification_system.get_notification(repository, 1, 2)

    assert notification == Notification(2, "Test Message")

def test_mark_all_read_happy_path(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message 1"))
    notification_system.add_notification(repository, 1, Notification(2, "Test Message 2"))
    notification_system.add_notification(repository, 1, Notification(3, "Test Message 3"))

    notification_system.mark_all_read(repository, 1)

    new_notifications = notification_system.get_new_notifications(repository, 1)

    assert len(new_notifications) == 0

    all_notifications = notification_system.get_user_notifications(repository, 1)

    assert set(all_notifications) == {
        Notification(1, "Test Message 1"),
        Notification(2, "Test Message 2"),
        Notification(3, "Test Message 3"),
    }


# Edge case tests
def test_get_new_notifications_user_not_found(notification_system, repository):
    with pytest.raises(UserNotFound):
        notification_system.get_new_notifications(repository, 999)

def test_mark_read_already_read_notification(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message"))
    notification_system.mark_read(repository, 1, 1)
    with pytest.raises(InvalidNotificationState):
        notification_system.mark_read(repository, 1, 1)

def test_delete_already_deleted_notification(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message"))
    notification_system.delete_notification(repository, 1, 1)
    with pytest.raises(InvalidNotificationState):
        notification_system.delete_notification(repository, 1, 1)

def test_mark_read_deleted_notification(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message"))
    notification_system.delete_notification(repository, 1, 1)
    with pytest.raises(InvalidNotificationState):
        notification_system.mark_read(repository, 1, 1)

def test_get_notification_deleted(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message"))
    notification_system.delete_notification(repository, 1, 1)
    with pytest.raises(InvalidNotificationState):
        notification_system.get_notification(repository, 1, 1)

def test_mark_read_notification_not_found(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message"))
    with pytest.raises(NotificationNotFound):
        notification_system.mark_read(repository, 1, 999)

def test_delete_notification_not_found(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message"))
    with pytest.raises(NotificationNotFound):
        notification_system.delete_notification(repository, 1, 999)

def test_user_not_found_exceptions(notification_system, repository):
    with pytest.raises(UserNotFound):
        notification_system.mark_read(repository, 999, 1)

    with pytest.raises(UserNotFound):
        notification_system.delete_notification(repository, 999, 1)

    with pytest.raises(UserNotFound):
        notification_system.get_user_notifications(repository, 999)

    with pytest.raises(UserNotFound):
        notification_system.get_notification(repository, 999, 1)

def test_notification_not_found_exception(notification_system, repository):
    notification_system.add_notification(repository, 1, Notification(1, "Test Message"))

    with pytest.raises(NotificationNotFound):
        notification_system.mark_read(repository, 1, 999)

    with pytest.raises(NotificationNotFound):
        notification_system.delete_notification(repository, 1, 999)

    with pytest.raises(NotificationNotFound):
        notification_system.get_notification(repository, 1, 999)

def test_mark_all_read_user_not_found(notification_system, repository):
    with pytest.raises(UserNotFound):
        notification_system.mark_all_read(repository, 999)
    