import os
from werkzeug.wsgi import SharedDataMiddleware
from werkzeug.wrappers import Request, Response
from werkzeug.exceptions import HTTPException, NotFound
from werkzeug.debug import DebuggedApplication
import json
from pony.orm.core import *
from datetime import datetime
import sys

with open(sys.argv[1]) as config_file:
    configuration = json.load(config_file)

db = Database()


class HumidityMeasure(db.Entity):
    timestamp = Required(datetime)
    value = Required(int)


class Event(db.Entity):
    timestamp = Required(datetime)
    kind = Required(str)


class WebService(object):

    def __init__(self):
        db.bind(provider="sqlite",
                filename=configuration["database"], create_db=True)
        db.generate_mapping(create_tables=False)

    def load_data(self):
        elements = list()
        with db_session:
            for m in select(m for m in HumidityMeasure):
                elements.append(
                    '{"value":' + str(m.value) + ',"timestamp":"' + str(m.timestamp) + '"}')
        result = '[' + ','.join(elements) + ']'
        return result

    def get_data_response(self):
        serialized = self.load_data()
        return Response(serialized, mimetype='application/json')

    def wsgi_app(self, environ, start_response):
        request = Request(environ)
        if (request.path == "/api/getdata"):
            if (request.method == "GET"):
                response = self.get_data_response()
            elif (request.method == "OPTIONS"):
                response = Response()
            else:
                raise NotFound()
            response.headers.add("Access-Control-Allow-Origin", "*")
            response.headers.add("Access-Control-Allow-Methods", "GET,PUT,POST,DELETE,PATCH")
            response.headers.add("Access-Control-Allow-Headers", "Content-Type, Authorization")
        else:
            raise NotFound()
        return response(environ, start_response)

    def __call__(self, environ, start_response):
        try:
            return self.wsgi_app(environ, start_response)
        except NotFound:
            return not_found(request)

if __name__ == '__main__':
    from werkzeug.serving import run_simple
    app = WebService()
    run_simple('0.0.0.0', 5000, app, use_debugger=True, use_reloader=True)