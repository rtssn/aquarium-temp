from typing import Union
from pydantic import BaseModel

from Model.Sensor import Sensor


class Telemetry(BaseModel):
    deviceId: str
    sensors: list[Sensor]
    ipAddress: str
    isFanOn: int
