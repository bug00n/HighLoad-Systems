import pytest
from NotificationService.App.UoW.notification_uow import UnitOfWorkNotification as UnitOfWork
from NotificationService.App.Repositories.repository import NotificationRepository
from NotificationService.App.Models.notification import Notification


@pytest.fixture
def repository():
    return NotificationRepository()


# Happy path tests
def test_change_uow(repository):
    UOW = UnitOfWork(repository)
    with UOW as uow:
        uow.repository.add(1, Notification(1, "Test Message 1"))

        assert uow.repository is not uow.repository_main
        assert uow.repository != uow.repository_main
    assert UOW.repository_main.notifications is UOW.repository.notifications

def test_uow_exit_commit(repository):
    UOW = UnitOfWork(repository)
    with UOW as uow:
        uow.repository.add(1, Notification(1, "Test Message 1"))

    notify = repository.get(1, 1)

    assert notify.notification == Notification(1, "Test Message 1")

def test_uow_exit_rollback(repository):
    UOW = UnitOfWork(repository)
    try:
        with UOW as uow:
            uow.repository.add(1, Notification(1, "Test Message 1"))
            raise Exception("Simulated Error")
    except Exception:
        pass

    with pytest.raises(Exception):
        repository.get(1, 1)

def test_uow_multiple_operations(repository):
    UOW = UnitOfWork(repository)
    with UOW as uow:
        uow.repository.add(1, Notification(1, "Test Message 1"))
        uow.repository.add(1, Notification(2, "Test Message 2"))

    notify1 = repository.get(1, 1)
    notify2 = repository.get(1, 2)

    assert notify1.notification == Notification(1, "Test Message 1")
    assert notify2.notification == Notification(2, "Test Message 2")

# Edge case tests
def test_uow_no_operations(repository):
    UOW = UnitOfWork(repository)
    with UOW as uow:
        pass

    with pytest.raises(Exception):
        repository.get(1, 1)

def test_uow_nested_operations(repository):
    UOW_outer = UnitOfWork(repository)
    with UOW_outer as uow_outer:
        uow_outer.repository.add(1, Notification(1, "Test Message 1"))
        UOW_inner = UnitOfWork(uow_outer.repository)
        with UOW_inner as uow_inner:
            uow_inner.repository.add(1, Notification(2, "Test Message 2"))

    notify1 = repository.get(1, 1)
    notify2 = repository.get(1, 2)

    assert notify1.notification == Notification(1, "Test Message 1")
    assert notify2.notification == Notification(2, "Test Message 2")
