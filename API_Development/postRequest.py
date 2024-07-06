from flask import Flask, request, jsonify
import json
import pymongo
from bson.objectid import ObjectId

class databaseHandler:
    def __init__(self, url, dbName, collectionName):
        self.url = url
        self.dbName = dbName
        self.collectionName = collectionName
        self.makeConnection()

    def makeConnection(self):
        try:
            self.client = pymongo.MongoClient(self.url)
            #print("server_info():", self.client.server_info())
        except Exception as e:   
            print("Could not connect to MongoDB")
            print(e)
        
        try:    
            self.db = self.client[self.dbName]
            self.collection = self.db[self.collectionName]
        except AttributeError as error:
            print ("Get MongoDB database and collection ERROR:", error)
            if self.client == None:
                print ("Client instance is invalid") 
    
    def readAndInsertJsonObjectToDb(self, ipJson):
        try:
            if isinstance(ipJson, list):
                self.collection.insert_many(ipJson)  
            else:
                self.collection.insert_one(ipJson)
        except Exception as e:   
            print("Failed to readAndInsertJsonObjectToDb")
            print(e)
    
    def getAllDataFromThisDbCollection(self):
        try:    
            result = self.collection.find()
            print('Reading from db:')
            for res in result:
                print(res)
            return result
        except Exception as e:   
            print("Failed to getAllDataFromThisDbCollection")
            print(e)
    
    def getObjectWithGivenId(self, id):
        query = {"_id": ObjectId(id)}
        filter = {"_id": 0}
        result = self.collection.find_one(query, filter)
        print('The queried object:')
        print(result)
        return result

    def getObjectWithGivenCriteria(self, objectCriteria, criteriaValue):
        try:
            query = {objectCriteria: criteriaValue}
            result = self.collection.find_one(query)
            print('The queried Criteria object:')
            print(result)
            return result
        except Exception as e:   
            print("Failed to getObjectWithGivenCriteria")
            print(e)
    
    def updateTheJsonObj(self, objectCriteria, criteriaValue, fieldToUpdate, value):
        try:
            result = self.collection.update_many( 
                {objectCriteria:criteriaValue}, 
                    { 
                        "$set":{ 
                            fieldToUpdate:value
                            }, 
                        "$currentDate":{"lastModified":True}                   
                    } 
                )
            return result.count()
        except Exception as e:   
            print("Failed to updateTheJsonObj")
            print(e)
    
    def deleteTheJsonObj(self, objectCriteria, criteriaValue):
        try:
            result = self.collection.delete_many({objectCriteria: criteriaValue})
        except Exception as e:   
           print("Failed to deleteTheJsonObj")
           print(e)

    def closeDbConnection(self):
        try:
            self.client.close()
        except Exception as e:   
           print("Failed to closeDbConnection")
           print(e)



app = Flask(__name__)



@app.route('/data', methods=['POST'])
def store_data():
    if request.is_json:
        data = request.get_json()
        
        uri = 'mongodb+srv://AlhadManure:AlhadManureMongo@cluster0.ckyvhwy.mongodb.net/'       
        dbName1 = "Alhad_Manure"
        collectionName1 = "data"
        mongodbObj = databaseHandler(url = uri, dbName=dbName1, collectionName= collectionName1)
        mongodbObj.readAndInsertJsonObjectToDb(data)
        mongodbObj.closeDbConnection()
        
        return jsonify({"message": "Data stored successfully"}), 201
    else:
        return jsonify({"error": "Request must be JSON"}), 400 



@app.route('/data/<ipId>', methods=['GET'])
def get_data(ipId):
    print("Retrieving:")
    uri = 'mongodb+srv://AlhadManure:AlhadManureMongo@cluster0.ckyvhwy.mongodb.net/'       
    dbName1 = "Alhad_Manure"
    collectionName1 = "data"
    mongodbObj = databaseHandler(url = uri, dbName=dbName1, collectionName= collectionName1)
    res = mongodbObj.getObjectWithGivenId(id = ipId )
    
    if res:
        return jsonify(res), 200
    else:
        return jsonify({"error": "Data not found"}), 404

def main():
    app.run(debug=True)

if __name__ == "__main__":
    main()