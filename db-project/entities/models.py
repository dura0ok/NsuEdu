from django.contrib.auth.models import AbstractUser
from django.db import models

from entities.project_models import Outlet


class CustomUser(AbstractUser):
    outlet = models.ForeignKey(
        Outlet,
        on_delete=models.CASCADE,
        related_name='users',
        null=True,
        blank=True,
        verbose_name='Здание'
    )

    class Meta:
        verbose_name = "Пользователь"
        verbose_name_plural = "Пользователи"
