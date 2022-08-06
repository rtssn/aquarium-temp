from datetime import datetime
from pydantic import BaseModel


class TempData(BaseModel):
    datetime: datetime
    sensor1Temp: float
    sensor2Temp: float
    isFanOn: int
