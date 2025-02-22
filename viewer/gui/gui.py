from PyQt6.QtWidgets import QVBoxLayout, QWidget, QComboBox
import pyqtgraph as pg
from telemetry_values.telemetry_values import Serie, TelemetryValues


class TelemetryGUI():
    def __init__(self, telemetry_values: TelemetryValues):
        self.series_list = {
            serie.name: serie for serie in telemetry_values.get_displayable_serie()}
        self.time_start = telemetry_values.time_start
        self.init_ui(telemetry_values.name)

    def init_ui(self, name):
        self.window = QWidget()
        self.window.setWindowTitle(name)

        self.layout = QVBoxLayout()
        self.window.setLayout(self.layout)

        self.combo_box = QComboBox()
        self.combo_box.addItems(self.series_list.keys())
        self.combo_box.currentTextChanged.connect(self.update_plot)
        self.layout.addWidget(self.combo_box)

        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setBackground("w")
        self.plot_widget.showGrid(x=True, y=True)
        self.layout.addWidget(self.plot_widget)

        self.update_plot(self.combo_box.currentText())

    def show(self) -> None:
        self.window.show()

    def update_plot(self, selected_series_name):
        self.plot_widget.clear()

        serie = self.series_list[selected_series_name]
        sorted_values = serie.get_sorted_values()

        timestamps = sorted_values.index.to_numpy()
        values = sorted_values.to_numpy()

        self.plot_widget.plot(timestamps, values, pen="b", name=serie.name)
        self.plot_widget.setTitle(f"{serie.name} ({serie.unit})")
        self.plot_widget.setLabel("left", f"Value ({serie.unit})")
        self.plot_widget.setLabel("bottom", "Time")

        self.plot_widget.setMouseEnabled(x=True, y=True)
