from pony import orm
from pony.orm.core import Required
from datetime import datetime

db = orm.Database()

class HumidityMeasure(db.Entity):
    timestamp = Required(datetime)
    value = Required(int)


class Event(db.Entity):
    timestamp = Required(datetime)
    kind = Required(str)