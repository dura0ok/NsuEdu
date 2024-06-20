from django.db import models
from django.db.models import UniqueConstraint


class FilmDevelopmentOrder(models.Model):
    code = models.CharField(
        max_length=255,
        unique=True,
        verbose_name='Код',
        help_text='Уникальный код пленки',
        db_comment='Уникальный код пленки'
    )

    service_order = models.ForeignKey(
        'ServiceOrder',
        on_delete=models.CASCADE,
        verbose_name='Заказ',
        help_text='Связь с заказом услуги',
        db_comment='Связь с заказом'
    )

    class Meta:
        db_table = 'film_development_orders'
        db_table_comment = 'Проявка пленок(вложено в чек заказа)'
        constraints = [
            UniqueConstraint(fields=['service_order', 'code'], name='unique_film_development_order'),
        ]
        verbose_name = 'Проявка пленок'
        verbose_name_plural = 'Проявка пленок'

    def __str__(self):
        return f'{self.service_order} - {self.code}'
