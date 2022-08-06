from pydantic import BaseModel


class Sensor(BaseModel):
    address: str
    temp: float
