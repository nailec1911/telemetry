from PyQt6.QtWidgets import QVBoxLayout, QWidget, QComboBox, QPushButton, QFileDialog, QErrorMessage
import pyqtgraph as pg
from telemetry_values.telemetry_values import TelemetryValues
from serie_parser.serie_parser import Parser


class TelemetryGUI():
    def __init__(self, telemetry_values: TelemetryValues):
        self.vals = telemetry_values
        self.series_list = {
            serie.name: serie for serie in telemetry_values.get_displayable_serie()}
        self.time_start = telemetry_values.time_start
        self.init_ui(telemetry_values.name)

    def init_ui(self, name):
        self.window = QWidget()
        self.window.setWindowTitle(name)

        self.layout = QVBoxLayout()
        self.window.setLayout(self.layout)

        # file selection
        self.load_button = QPushButton("Load File")
        self.load_button.clicked.connect(self.load_file)
        self.layout.addWidget(self.load_button)

        # serie selection
        self.combo_box = QComboBox()
        self.combo_box.addItems(self.series_list.keys())
        self.combo_box.currentTextChanged.connect(self.update_plot)
        self.layout.addWidget(self.combo_box)

        # plot
        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setBackground("w")
        self.plot_widget.showGrid(x=True, y=True)
        self.layout.addWidget(self.plot_widget)

        self.update_plot(self.combo_box.currentText())

    def show(self) -> None:
        self.window.show()

    def update_plot(self, selected_series_name):
        self.plot_widget.clear()

        if selected_series_name not in self.series_list:
            return

        serie = self.series_list[selected_series_name]
        sorted_values = serie.get_sorted_values()

        timestamps = sorted_values.index.to_numpy()
        values = sorted_values.to_numpy()

        self.plot_widget.plot(timestamps, values, pen=pg.mkPen(
            'b', width=5), symbol='o', symbolSize=7, name=serie.name)
        self.plot_widget.setTitle(f"{serie.name} ({serie.unit})")
        self.plot_widget.setLabel("left", f"Value ({serie.unit})")
        self.plot_widget.setLabel("bottom", "Time")

        self.plot_widget.setMouseEnabled(x=True, y=True)

    def load_file(self):
        file_name, _ = QFileDialog.getOpenFileName(
            self.window, "Open Telemetry File", "", "Telemetry Files : *.bin *.log;;All Files (*)")

        if not file_name:
            return

        try:
            parsed = Parser(file_name)
            loaded_values: TelemetryValues = parsed.get_datas()
            print()
            print(f"Telemetry infos from file '{file_name}' properly loaded.")
            loaded_values.display()
            self.vals = loaded_values
        except Exception as e:
            error_dialog = QErrorMessage()
            error_dialog.showMessage(f"Failed to load file:\n\n{str(e)}")
            self.layout.addWidget(error_dialog)
            return
        self.series_list = {
            serie.name: serie for serie in self.vals.get_displayable_serie()}
        self.time_start = self.vals.time_start

        self.combo_box.clear()
        self.combo_box.addItems(self.series_list.keys())
        if self.series_list:
            self.update_plot(self.combo_box.currentText())
