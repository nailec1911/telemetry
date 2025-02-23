import pandas as pd


class Serie:
    def __init__(self, name: str, serie_id: int, timestamp: int, unit: str, value_type: type):
        self.name: str = name
        self.unit: str = unit
        self.value_type: type = value_type
        self.id = serie_id
        self.timestamp: int = timestamp
        self.values = pd.Series(dtype=value_type)
        return

    def add_value(self, timestamp, value):
        self.values.at[timestamp] = value
        return

    def print_values(self):
        for timestamp, value in self.values.items():
            print(f"{self.name} [{timestamp}]: {value}")
        return

    def get_sorted_values(self) -> list:
        return self.values.sort_index()

    def get_type(self) -> type:
        return self.value_type

    def __str__(self) -> str:
        return f"Serie '{self.name}' " \
            f"(ID: {self.id}, Unit: {self.unit}, type: {self.value_type}, Start time: {self.timestamp})"

    def __repr__(self):
        return f"Serie '{self.name}' " \
            f"(ID: {self.id}, Unit: {self.unit}, type: {self.value_type}, Start time: {self.timestamp})"


class TelemetryValues():
    def __init__(self, name, timestamp):
        self.name = name
        self.time_start = timestamp
        self.series = {}

    def add_serie(self, name: str, serie_id: int, timestamp: int, unit: str, value_type: type) -> None:
        self.series[serie_id] = Serie(
            name, serie_id, timestamp, unit, value_type)
        return

    def add_value(self, serie_id: int, timestamp: int, value):
        if serie_id not in self.series:
            raise ValueError("Error: trying to add a value in unknow serie")
        self.series[serie_id].add_value(timestamp, value)
        return

    def is_serie_logged(self, serie_id: int) -> bool:
        return serie_id in self.series

    def get_serie(self, serie_id: int) -> Serie:
        if serie_id not in self.series:
            raise ValueError("Error: trying to add a value in unknow serie")
        return self.series[serie_id]

    def get_displayable_serie(self) -> list[Serie]:
        return list(filter(lambda serie: serie.get_type() != str, self.series.values()))

    def display(self) -> None:
        print(self.name, self.time_start)
        for _, serie in self.series.items():
            print(serie)
            serie.print_values()
        return
