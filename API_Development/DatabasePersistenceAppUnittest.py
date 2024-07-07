import unittest
import json
from bson.json_util import dumps, loads
from DataBasePersistenceApp import app, databaseHandler

class FlaskTestCase(unittest.TestCase):
    def setUp(self):
        # Set up test variables and initialize the database.
        self.app = app.test_client()
        self.app.testing = True

        # Initialize the database
        self.uri = 'mongodb+srv://AlhadManure:AlhadManureMongo@cluster0.ckyvhwy.mongodb.net/'       
        self.dbToUse = "Alhad_Manure"
        self.collectionToUse = "data"

        self.dbObj = databaseHandler(url = self.uri, dbName=self.dbToUse, collectionName = self.collectionToUse)

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
        response = self.app.post('/data', data=json.dumps({'key': 'value'}), content_type='application/json')
        self.assertEqual(response.status_code, 201)
        self.assertIn(b'Data stored successfully', response.data)

    def test_get_data(self):
        # Test retrieving JSON data.
        # First, store data to ensure there is something to get
        self.app.post('/data', data=json.dumps({'keyGet1': 'valueGet1'}), content_type='application/json')
        self.app.post('/data', data=json.dumps({'keyGet2': 'valueGet2'}), content_type='application/json')
        
        # Get ObjectIds from our db
        ids = self.getIds()
             
        response = self.app.get('/data/' + ids[0])
        self.assertEqual(response.status_code, 200)
        self.assertIn(b'{"keyGet1":"valueGet1"}', response.data)
          
    def test_update_data(self):
        # Test updating JSON data.
        # First, store data to ensure there is something to update
        self.app.post('/data', data=json.dumps({'key': 'value'}), content_type='application/json')
        
        # Get ObjectIds from our db
        ids = self.getIds()

        response = self.app.put('/data/' + ids[1], data=json.dumps({'key': 'new_value'}), content_type='application/json')
        self.assertEqual(response.status_code, 200)
        self.assertIn(b'Data updated successfully', response.data)

    def test_delete_data(self):
        # Test deleting JSON data.
        # First, store data to ensure there is something to delete
        self.app.post('/data', data=json.dumps({'key': 'value'}), content_type='application/json')
        
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