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
        sql = "SELECT `datetime`, `sensor1Temp`, `sensor2Temp`, `isFanOn`, `ipAddress` FROM `temp`"
        cursor = db.cursor(buffered=True)

        cursor.execute(sql)
        for (datetime, sensor1Temp, sensor2Temp, isFanOn, ipAddress) in cursor:
            data = {
                'datetime': datetime,
                'deviceId': '82d08228a35148b59ad4a11fcefef1d5',
                'sensors': [
                    {
                        'address': '28 ff 70 b6 a4 16 4 f2',
                        'temp': sensor1Temp
                    },
                    {
                        'address': '28 ff 54 bf b4 16 5 92',
                        'temp': sensor2Temp
                    }
                ],
                'ipAddress': ipAddress,
                'isFanOn': isFanOn
            }
            mongodb.aquarium.insert_one(data)
        cursor.close()

except Exception as e:
    print(f"Error Occurred: {e}")

finally:
    if db is not None and db.is_connected():
        db.close()
