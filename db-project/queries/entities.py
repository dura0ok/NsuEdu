import json
from dataclasses import dataclass

dictType = dict[str, any]


@dataclass(frozen=True)
class QueryResult:
    columns: list[dictType]
    data: list[dictType]

    def to_dict(self) -> dict:
        return {"columns": self.columns, "data": self.data}

    def to_json(self) -> str:
        return json.dumps(self.to_dict())

    def add_value_to_column_by_field(self, field_name: str, key: str, data: any) -> None:
        for column in self.columns:
            if column["field"] == field_name:
                column[key] = data
