from flask import Flask, jsonify, request
from flask_cors import CORS
import json
from pony.orm.core import db_session, select
from datetime import timedelta
import sys
from model import *

with open('config.json') as config_file:
    configuration = json.load(config_file)

db.bind(provider="sqlite", filename=configuration["database"], create_db=True)
db.generate_mapping(create_tables=False)

app = Flask(__name__)
CORS(app)

def loadData(days):
    elements = list()
    with db_session:
        for m in select(m for m in HumidityMeasure if m.timestamp + timedelta(days=days) >= datetime.now()):
            elements.append({
                "value": m.value,
                "timestamp": str(m.timestamp)
            })
    return elements


@app.route('/api/getdata', methods=['GET'])
def getData():
    days = request.args.get('days', '2')
    data = loadData(int(days))
    return jsonify(data)
