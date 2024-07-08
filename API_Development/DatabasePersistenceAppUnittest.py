import unittest
import json
from bson.json_util import dumps, loads
from DataBasePersistenceApp import app, databaseHandler

class FlaskTestCase(unittest.TestCase):
    def setUp(self):
        # Set up test variables and initialize the database.
        self.app = app.test_client()
        self.app.testing = True

        self.dbObj = databaseHandler()
        # First, store data to ensure there is something in db
        ipJson = [{"key1":"value1"}, {"key2":"value2"}, {"key3":"value3"}, {"key4":"value4"}]
        self.dbObj.insertJsonObjectToDb(ipJson)

    def getIds(self):
        # Get all data from db
        dbData = list(self.dbObj.collection.find())

        # Converting to the JSON 
        jsonData = json.loads(dumps(dbData, indent = 2))

        # list all ObjectIds from db data
        idList = []
        for data in jsonData:
            idStr = data['_id']
            idList.append(idStr['$oid'])
        return idList

    def test_store_data(self):
        # Test storing JSON data.
        response = self.app.post('/data', data=json.dumps({'key5': 'value5'}), content_type='application/json')

        self.assertEqual(response.status_code, 201)
        self.assertIn(b'Data stored successfully', response.data)

    def test_get_data(self):
        # Test retrieving JSON data.
        # Get ObjectIds from our db
        ids = self.getIds()
             
        response = self.app.get('/data/' + ids[0])
        self.assertEqual(response.status_code, 200)
        self.assertIn(b'{"key1":"value1"}', response.data)
          
    def test_update_data(self):
        # Test updating JSON data.      
        # Get ObjectIds from our db
        ids = self.getIds()

        response = self.app.put('/data/' + ids[1], data=json.dumps({'key3Up': 'new_value3Up'}), content_type='application/json')
        self.assertEqual(response.status_code, 200)
        self.assertIn(b'Data updated successfully', response.data)

    def test_delete_data(self):
        # Test deleting JSON data.    
        # Get ObjectIds from our db
        ids = self.getIds()

        response = self.app.delete('/data/' + ids[2])
        self.assertEqual(response.status_code, 200)
        self.assertIn(b'Data deleted successfully', response.data)

    def test_get_nonexistent_data(self):
        # Test retrieving non-existent JSON data.
        response = self.app.get('/data/999')
        self.assertEqual(response.status_code, 404)
        self.assertIn(b'Data not found', response.data)

    def test_update_nonexistent_data(self):
        # Test updating non-existent JSON data.
        response = self.app.put('/data/999', data=json.dumps({'key': 'new_value'}), content_type='application/json')
        self.assertEqual(response.status_code, 404)
        self.assertIn(b'Data not found', response.data)

    def test_delete_nonexistent_data(self):
        # Test deleting non-existent JSON data.
        response = self.app.delete('/data/999')
        self.assertEqual(response.status_code, 404)
        self.assertIn(b'Data not found', response.data)

    def test_store_invalid_json(self):
        # Test storing invalid JSON data.
        response = self.app.post('/data', data='invalid_json', content_type='application/json')
        self.assertEqual(response.status_code, 400)
        self.assertIn(b'400 Bad Request', response.data)
    
        
if __name__ == '__main__':
    unittest.main()