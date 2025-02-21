"""parsing of the log file
"""
import struct
import pandas as pd


class Serie:
    def __init__(self, name: str, serie_id: int, timestamp: int, unit: str, value_type: type):
        self.name: str = name
        self.unit: str = unit
        self.value_type: type = value_type
        self.id = serie_id
        self.timestamp: int = timestamp
        self.values = pd.Series()
        return

    def add_value(self, timestamp, value):
        pass

    def __str__(self) -> str:
        return f"Serie '{self.name}' " \
            f"(ID: {self.id}, Unit: {self.unit}, Start time: {self.timestamp})"

    def __repr__(self):
        return f"Serie '{self.name}' " \
            f"(ID: {self.id}, Unit: {self.unit}, Start time: {self.timestamp})"


class Parser:
    SIZE_ID = 2
    SIZE_NAME_SIZE = 1
    SIZE_MIN_SERIE = 28
    SIZE_MIN_VALUE = "fdsfdsf"
    SIZE_TIMESTAMP = 8

    def get_string(self) -> str:
        if len(self.file) < 1:
            raise ValueError("Invalid string type")
        size: int = struct.unpack("B", self.file[:1])[0]
        if len(self.file) < size + 1:
            raise ValueError("Invalid string type")
        _, res = struct.unpack(f"=B{size}s", self.file[:size+1])
        self.file = self.file[size + 1:]

        return res.decode("ascii").rstrip("\x00")

    def __init__(self, file: bytes) -> None:
        self.file = file
        self.series: dict[int:Serie] = {}

        self.name = self.get_string()
        self.timestamp: int = struct.unpack(
            "Q", self.file[:self.SIZE_TIMESTAMP])[0]
        print(self.name, self.timestamp)
        self.file = self.file[self.SIZE_TIMESTAMP:]

        while len(file) > 1:
            serie_id: int = struct.unpack("H", self.file[:self.SIZE_ID])[0]
            self.file = self.file[self.SIZE_ID:]
            if serie_id == 0:
                self.parse_serie()
            else:
                self.parse_value(serie_id)
        print(len(file))
        return

    def parse_value(self, serie_id):
        pass

    def parse_serie(self):
        if len(self.file) < self.SIZE_MIN_SERIE:
            raise ValueError(
                "Error in serie parsing, file does not contains enought informations")
        name_size: int = self.file[0]

        if len(self.file) < self.SIZE_MIN_SERIE + name_size:
            raise ValueError(
                "Error in serie parsing, serie name does not match with the size")

        data_format: str = f"=B {name_size}s H Q B 16s"
        data_size = struct.calcsize(data_format)
        _, name, serie_id, timestamp, type_val, unit = struct.unpack(
            data_format, self.file[:data_size])
        name = name.decode("ascii").rstrip("\x00")
        unit = unit.decode("ascii").rstrip("\x00")
        type_val = str if type_val == 0 else int
        self.series[serie_id] = Serie(
            name, serie_id, timestamp, unit, type_val)
        self.file = self.file[data_size:]
        print(self.series)
