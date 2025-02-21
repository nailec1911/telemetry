"""parsing of the log file
"""
import pandas as pd


class Serie:
    def __init__(self, name: str, unit: str, value_type: type):
        self.name: str = name
        self.unit: str = unit
        self.value_type: type = value_type
        self.values = pd.Series()
        return

    def add_value(self, timestamp, value):
        pass


class Parser:
    def __init__(self, file: str):
        self.series: list[Serie] = []
