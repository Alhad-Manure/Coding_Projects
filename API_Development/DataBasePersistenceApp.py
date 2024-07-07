from flask import Flask, request, jsonify
import json
import pymongo
from bson.objectid import ObjectId

# keeping this parameters predefined as of now THese can be sent as part of url
# When this aplication is further customised
uri = 'mongodb+srv://AlhadManure:AlhadManureMongo@cluster0.ckyvhwy.mongodb.net/'       
dbToUse = "Alhad_Manure"
collectionToUse = "data"

# This class will take care of database handalling
# To-Do make it as singleton
class databaseHandler:
    
    def __init__(self, url, dbName, collectionName):
        self.url = url
        self.dbName = dbName
        self.collectionName = collectionName
        self.makeConnection()

    def __del__(self):
        # Make sure that db connection is closed at end of scope or when object is destroyed.
        self.closeDbConnection()

    # Method to set connection with our database
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
    
    # Method to insert new json object in database
    def insertJsonObjectToDb(self, ipJson):
        try:
            if isinstance(ipJson, list):
                self.collection.insert_many(ipJson)  
            else:
                self.collection.insert_one(ipJson)
        except Exception as e:   
            print("Failed to insertJsonObjectToDb")
            print(e)
    
    # Method to get all data from database
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
    
    # Method to get json object of given ID from database
    def getObjectWithGivenId(self, id):
        try:
            query = {"_id": ObjectId(id)}
            # Exluding _id field while retreiving the data
            filter = {"_id": 0}
            result = self.collection.find_one(query, filter)
            print('The queried object:')
            print(result)
            return result
        except Exception as e:   
            print("Failed to getObjectWithGivenId")
            print(e)

    # Method to get json object containing given key and value from database
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
    
    # Method to update json object of given ID from database
    def updateTheJsonObjOfId(self, id, dataToUpdate):
        try:
            totCount = 0
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
            print("Failed to updateTheJsonObj")
            print(e)

    # Method to update json object containing given key and value from database
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
    
    # Method to delete json object of given ID from database
    def deleteTheJsonObjOfId(self, id):
        try:
            result = self.collection.delete_many({"_id": ObjectId(id)})
            return result.deleted_count

        except Exception as e:   
           print("Failed to deleteTheJsonObj")
           print(e)

    # Method to delete json object containing given key and value from database
    def deleteTheJsonObj(self, objectCriteria, criteriaValue):
        try:
            result = self.collection.delete_many({objectCriteria: criteriaValue})
        except Exception as e:   
           print("Failed to deleteTheJsonObj")
           print(e)

    # Method to close the connection with db
    def closeDbConnection(self):
        try:
            self.client.close()
        except Exception as e:   
           print("Failed to closeDbConnection")
           print(e)

# Creating Flask application
app = Flask(__name__)

# POST request handling
@app.route('/data', methods=['POST'])
def store_data():
    if request.is_json:
        data = request.get_json()
        
        mongodbObj = databaseHandler(url = uri, dbName=dbToUse, collectionName= collectionToUse)
        mongodbObj.insertJsonObjectToDb(data)
        
        return jsonify({"message": "Data stored successfully"}), 201
    else:
        return jsonify({"error": "Request must be JSON"}), 400 

# GET request handling
@app.route('/data/<ipId>', methods=['GET'])
def get_data(ipId):
    print("Retrieving:")
    mongodbObj = databaseHandler(url = uri, dbName=dbToUse, collectionName= collectionToUse)
    res = mongodbObj.getObjectWithGivenId(id = ipId )
    
    if res:
        return jsonify(res), 200
    else:
        return jsonify({"error": "Data not found"}), 404

# PUT request handling
@app.route('/data/<ipId>', methods=['PUT'])
def update_data(ipId):
    print("Updating:")

    if request.is_json:
        data = request.get_json()

        mongodbObj = databaseHandler(url = uri, dbName=dbToUse, collectionName= collectionToUse)
        json_str = str(data)
        res = mongodbObj.updateTheJsonObjOfId( id=ipId, dataToUpdate=data)
        
        if res > 0:
            return jsonify({"message": "Data updated successfully"}), 200
        else:
            return jsonify({"error": "Data not found"}), 404
    else:
        return jsonify({"error": "Request must be JSON"}), 400


# DELETE request handling
@app.route('/data/<ipId>', methods=['DELETE'])
def delete_data(ipId):

    mongodbObj = databaseHandler(url = uri, dbName=dbToUse, collectionName= collectionToUse)
    result = mongodbObj.deleteTheJsonObjOfId( id=ipId )    

    if result > 0:
        return jsonify({"message": "Data deleted successfully"}), 200
    else:
        return jsonify({"error": "Data not found"}), 404

def main():
    app.run(debug=True)

if __name__ == "__main__":
    main()