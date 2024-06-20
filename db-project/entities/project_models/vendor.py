from django.db import models


class Vendor(models.Model):
    name = models.CharField(
        unique=True,
        max_length=100,
        help_text='Введите название компании (поставщика)',
        db_comment='Название компании (поставщика)'
    )

    class Meta:
        db_table = 'vendors'
        db_table_comment = 'Поставщики'
        verbose_name = 'Поставщик'
        verbose_name_plural = 'Поставщики'

    def __str__(self):
        return f'{self.name}'
