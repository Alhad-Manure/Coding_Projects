import json
import pymongo
from bson.objectid import ObjectId
from flask import  jsonify

# This class will take care of database handalling
# To-Do make it as singleton
class databaseHandler:
    
    def __init__(self):
        self.getCredentials()
        self.makeConnection()

    def __del__(self):
        # Make sure that db connection is closed at end of scope or when object is destroyed.
        self.closeDbConnection()

    # Method to read db credentials from config file
    def getCredentials(self):
        try:
            # Opening JSON file
            f = open('dbConfig.json')
            
            # returns JSON object as 
            # a dictionary
            data = json.load(f)
            
            # Iterating through the json
            # list
            for d in data['dbCred']:
                self.url = d["url"]
                self.dbName = d["dbName"]
                self.collectionName = d["collectionName"]
            
            # Closing file
            f.close()
        except Exception as e:   
            print("Failed to parse dbConfig.json", e)

    # Method to set connection with our database
    def makeConnection(self):
        try:
            self.client = pymongo.MongoClient(self.url)
            print("server_info():", self.client.server_info())
        except Exception as e:   
            print("Could not connect to MongoDB", e)
        
        try:    
            self.db = self.client[self.dbName]
            self.collection = self.db[self.collectionName]
        except AttributeError as error:
            print ("Get MongoDB database and collection ERROR:", error)
            if self.client == None:
                print ("Client instance is invalid") 
    
    # Method to insert new json object in database
    def insertJsonObjectToDb(self, ipJson):
        try:
            if isinstance(ipJson, list):
                self.collection.insert_many(ipJson)  
            else:
                self.collection.insert_one(ipJson)
        except Exception as e:   
            print("Failed to insertJsonObjectToDb", e)
    
    # Method to get all data from database
    def getAllDataFromThisDbCollection(self):
        try:    
            result = self.collection.find()
            print('Reading from db:')
            for res in result:
                print(res)
            return result
        except Exception as e:   
            print("Failed to getAllDataFromThisDbCollection", e)
    
    # Method to get json object of given ID from database
    def getObjectWithGivenId(self, id):
        try:
            query = {"_id": ObjectId(id)}
            # Exluding _id field while retreiving the data
            filter = {"_id": 0}
            result = self.collection.find_one(query, filter)
            return result
        except Exception as e:   
            print("Failed to getObjectWithGivenId", e)

    # Method to update json object of given ID from database
    def updateTheJsonObjOfId(self, id, dataToUpdate):
        try:
            totCount = 0

            if(not ObjectId.is_valid(id)):     
                   return 0
            
            for key in dataToUpdate:
                result = self.collection.update_many( 
                    {"_id": ObjectId(id)}, 
                        { 
                            "$set":{ 
                                 str(key):str(dataToUpdate[key])
                                }, 
                            "$currentDate":{"lastModified":True}                   
                        } 
                    )
                totCount += result.matched_count
            
            return totCount

        except Exception as e:   
            print("Failed to updateTheJsonObj", e)
            return jsonify({"error": "Data not found"}), 404

    # Method to delete json object of given ID from database
    def deleteTheJsonObjOfId(self, id):
        try:
            if(not ObjectId.is_valid(id)):     
                   return 0

            result = self.collection.delete_many({"_id": ObjectId(id)})
            return result.deleted_count

        except Exception as e:   
           print("Failed to deleteTheJsonObj", e)

    # Method to close the connection with db
    def closeDbConnection(self):
        try:
            self.client.close()
        except Exception as e:   
           print("Failed to closeDbConnection", e)