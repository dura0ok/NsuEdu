from django.core.validators import MinValueValidator
from django.db import models
from django.db.models import UniqueConstraint

from entities.project_models import Item


class ServiceTypeNeededItem(models.Model):
    item = models.OneToOneField(
        Item,
        on_delete=models.CASCADE,
        verbose_name='Товар',
        primary_key=True,
        help_text='Необходимый предмет'
    )

    service_type = models.ForeignKey(
        'ServiceType',
        on_delete=models.CASCADE,
        verbose_name='Тип',
        help_text='Тип услуги, к которому относится предмет'
    )

    count = models.IntegerField(
        verbose_name='Количество',
        help_text='Количество необходимых предметов',
        validators=[MinValueValidator(0)]
    )

    class Meta:
        db_table = 'service_types_needed_items'
        constraints = [
            UniqueConstraint(
                fields=['item', 'service_type'],
                name='unique_service_type_needed_item')
        ]
        verbose_name = 'Необходимые предметы для типа услуги'
        verbose_name_plural = 'Необходимые предметы для типов услуг'

    def __str__(self):
        return f'{self.item} - {self.service_type}'
