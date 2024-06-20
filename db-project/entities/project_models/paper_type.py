from django.db import models


class PaperType(models.Model):
    name = models.CharField(
        verbose_name='Наименование',
        unique=True,
        max_length=30,
        help_text='Название типа бумаги'
    )

    class Meta:
        verbose_name = 'Тип бумаги'
        verbose_name_plural = 'Типы бумаги'
        db_table = 'paper_types'
        db_table_comment = 'Тип бумаги'

    def __str__(self):
        return f'{self.name}'
