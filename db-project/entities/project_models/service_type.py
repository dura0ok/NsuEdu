from django.core.validators import MinValueValidator
from django.db import models


class ServiceType(models.Model):
    name = models.CharField(
        max_length=100,
        help_text='Название услуги'
    )

    price = models.DecimalField(
        max_digits=15,
        decimal_places=2,
        verbose_name='Цена',
        help_text='Цена услуги',
        validators=[MinValueValidator(0)]
    )

    class Meta:
        db_table = 'service_types'
        verbose_name = 'Тип услуги'
        verbose_name_plural = 'Типы услуг'

    def __str__(self):
        return f'{self.name}'
