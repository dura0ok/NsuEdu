from django.db import models
from django.db.models import UniqueConstraint


class Outlet(models.Model):
    type = models.ForeignKey(
        'OutletType',
        models.CASCADE,
        db_comment='Тип здания',
        help_text='Выберите тип здания.'
    )

    address = models.CharField(
        max_length=200,
        db_comment='Адрес',
        verbose_name='Адрес',
        help_text='Введите адрес здания.'
    )

    num_workers = models.IntegerField(
        verbose_name='Количество работников',
        help_text='Количество работников'
    )

    class Meta:
        db_table = 'outlets'
        constraints = [
            UniqueConstraint(fields=['type', 'address'], name='unique_type_address')
        ]
        db_table_comment = 'Здания'
        verbose_name = "Здание"
        verbose_name_plural = "Здания"

    def __str__(self):
        return f"{self.type} - {self.address}"
