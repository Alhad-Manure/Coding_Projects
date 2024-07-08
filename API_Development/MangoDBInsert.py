import json
import pymongo
from bson.objectid import ObjectId

uri = 'mongodb+srv://AlhadManure:AlhadManureMongo@cluster0.ckyvhwy.mongodb.net/' 

client = pymongo.MongoClient(uri)
db = client["Alhad_Manure"]
Collection = db["data"]

ipJson = '[{"one":"1.232.123","two":"test","three":1242,"four":"2,4","five":"test","six":"test","seven":"test"},{"one":"3.322.876","two":"test","three":1312,"four":"3,4","five":"test","six":"test","seven":"test"},{"one":"1.232.123","two":"test","three":1242,"four":"2,4","five":"test","six":"test","seven":"test"},{"one":"3.322.876","two":"test","three":1312,"four":"3,4","five":"test","six":"test","seven":"test"}]'

def getDbCredentials():
    # Opening JSON file
    f = open('dbConfig.json')
    
    # returns JSON object as 
    # a dictionary
    data = json.load(f)
    
    # Iterating through the json
    # list
    for d in data['dbCred']:
        uri = d["url"]
        dbToUse = d["dbName"]
        collectionToUse = d["collectionName"]
    
    # Closing file
    f.close()

def readAndStoreJsonObject(ipJson):
    jsonObj = json.loads(ipJson)
    #print('Storing data to db:')
    if isinstance(jsonObj, list):
        Collection.insert_many(jsonObj)  
    else:
        Collection.insert_one(jsonObj)
    #print('Done with Storing')

def printDataFromDb():
    result = client["Alhad_Manure"]["data"].find()
    print('Readiing from db:')
    for res in result:
        print(res)

def getObjectWithGivenId(id):
    query = {"_id": ObjectId(id)}
    filter = {"_id": 0}
    result = Collection.find_one(query, filter)
    print('The queried object:')
    print(result)

def getObjectWithGivenCriteria(objectCriteria, criteriaValue):
    query = {objectCriteria: criteriaValue}
    #filter = {"_id": 0}
    result = Collection.find_one(query)
    print('The queried Criteria object:')
    print(result)    

def updateTheJsonObj(objectCriteria, criteriaValue, fieldToUpdate, value):
    result = Collection.update_many( 
        {objectCriteria:criteriaValue}, 
        { 
                "$set":{ 
                        fieldToUpdate:value
                        }, 
                "$currentDate":{"lastModified":True}                   
                } 
        )

def deleteTheJsonObj(objectCriteria, criteriaValue):
    result = Collection.delete_many({objectCriteria: criteriaValue})

def main():
    '''
    readAndStoreJsonObject(ipJson)
    printDataFromDb()
    getObjectWithGivenId("668976b6c7b64cc8b14e7dec")
    getObjectWithGivenCriteria(objectCriteria='one' , criteriaValue='3.322.876')
    updateTheJsonObj(objectCriteria='one' , criteriaValue='1.232.123', fieldToUpdate='seven', value='alhadSeven')
    deleteTheJsonObj(objectCriteria='one', criteriaValue='3.322.876')
    getObjectWithGivenCriteria(objectCriteria='one', criteriaValue='3.322.876')
    '''
    getDbCredentials()
    
if __name__ == "__main__":
    main()

