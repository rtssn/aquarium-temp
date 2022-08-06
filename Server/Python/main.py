from fastapi import FastAPI
from pymongo import MongoClient
from datetime import datetime, timedelta
import mysql.connector
import json


from Model.Telemetry import Telemetry
from Model.TempData import TempData

configFile = open('./config.json', 'r')
config = json.load(configFile)

print(config['aquarium']['user'])

db = None

app = FastAPI()

mongodbClient = MongoClient('localhost', 27017)
mongodb = mongodbClient['iot']


@app.get("/", response_model=list[TempData])
async def root():
    ret: list[TempData] = list()
    try:
        db = mysql.connector.connect(
            user=config['aquarium']['user'],  # ユーザー名
            password=config['aquarium']['password'],  # パスワード
            host=config['aquarium']['host'],  # ホスト名(IPアドレス）
            database=config['aquarium']['database']  # データベース名
        )

        if db.is_connected:
            print("Connected!")
            sql = "SELECT `datetime`, `sensor1Temp`, `sensor2Temp`, `isFanOn` FROM `temp` WHERE `datetime` > ( NOW( ) - INTERVAL 1 DAY ) ORDER BY `id` ASC LIMIT 3600;"
            cursor = db.cursor(buffered=True)

            cursor.execute(sql)
            for (datetime, sensor1Temp, sensor2Temp, isFanOn) in cursor:
                data = TempData(
                    datetime=datetime,
                    sensor1Temp=sensor1Temp,
                    sensor2Temp=sensor2Temp,
                    isFanOn=isFanOn
                )
                ret.append(data)
            cursor.close()

    except Exception as e:
        print(f"Error Occurred: {e}")

    finally:
        if db is not None and db.is_connected():
            db.close()

    return ret


@app.get('/get', response_model=list[Telemetry])
async def get():
    ret = list[Telemetry]()
    now = datetime.now()
    beoforeOneDay = now - timedelta(days=1)
    result = mongodb.get_collection('aquarium').find(filter={
        'datetime': {'$gt': beoforeOneDay}
    })

    print(type(result))
    for doc in result:
        ret.append(doc)

    return ret


@ app.post('/post')
async def post(data: Telemetry):
    mongodb.aquarium.insert_one(data)
    return data
