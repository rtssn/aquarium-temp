import mysql.connector
from pymongo import MongoClient

try:
    db = mysql.connector.connect(
        user='',  # ユーザー名
        password='',  # パスワード
        host='',  # ホスト名(IPアドレス）
        database=''  # データベース名
    )

    mongodbClient = MongoClient('localhost', 27017)
    mongodb = mongodbClient['iot']

    if db.is_connected:
        print("Connected!")
        sql = "SELECT `datetime`, `temperature`, `humidity`, `pressure` FROM `ObservationResults`"
        cursor = db.cursor(buffered=True)

        cursor.execute(sql)
        for (datetime, temperature, humidity, pressure) in cursor:
            data = {
                'datetime': datetime,
                'temperature': temperature,
                'humidity': humidity,
                'pressure': pressure
            }
            mongodb.bme280.insert_one(data)
        cursor.close()

except Exception as e:
    print(f"Error Occurred: {e}")

finally:
    if db is not None and db.is_connected():
        db.close()
