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
            print("server_info():", self.client.server_info())
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
            jsonObj = json.loads(ipJson)
            
            if isinstance(jsonObj, list):
                self.collection.insert_many(jsonObj)  
            else:
                self.collection.insert_one(jsonObj)
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

    

     
          