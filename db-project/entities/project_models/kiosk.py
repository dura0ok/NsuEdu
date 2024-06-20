from django.db import models

from entities.project_models import Branch


class Kiosk(models.Model):
    outlet = models.OneToOneField(
        'Outlet',
        models.CASCADE,
        primary_key=True,
        verbose_name='Адрес',
        db_comment='Здание где расположен',
        help_text='Связь со зданием в котором расположен киоск',
    )
    branch_outlet = models.ForeignKey(
        Branch,
        on_delete=models.CASCADE,
        verbose_name='Филиал',
        db_comment='Киоск связан с филиалом',
        help_text='Связь с филиалом, к которому прикреплен киоск'
    )

    class Meta:
        db_table = 'kiosks'
        db_table_comment = 'Киоск'
        verbose_name = 'Киоск'
        verbose_name_plural = 'Киоски'

    def __str__(self):
        return f'{self.outlet.address}'
