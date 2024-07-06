import json
import pymysql

con = pymysql.connect(host = 'host', port = 3306, user = 'user', passwd = 'pass', db = 'db')
cursor = con.cursor()

ipJson = '[{"one":"1.232.123","two":"test","three":1242,"four":"2,4","five":"test","six":"test","seven":"test"},{"one":"3.322.876","two":"test","three":1312,"four":"3,4","five":"test","six":"test","seven":"test"},{"one":"1.232.123","two":"test","three":1242,"four":"2,4","five":"test","six":"test","seven":"test"},{"one":"3.322.876","two":"test","three":1312,"four":"3,4","five":"test","six":"test","seven":"test"}]'

def constructInsertQuery(obj):
    query = "INSERT INTO test ("
    
    for key in obj:
        query += key + ", "
    query = query[:-2]
    query += ') VALUES ('
    
    for key in obj:
        query += str(obj[key]) + ", "
    query = query[:-2]    
    query += ')'

    return query

def readAndStoreJsonObject(ipJson):
    jsonObj = json.loads(ipJson)
    print('Hello')
    colNo = 1   
    for obj in jsonObj:       
        # debug code:
        '''
        for key in obj:
            print(key, " : ", obj[key])
        '''           
        insertQuery = constructInsertQuery(obj)
        # debug code:
        #print(insertQuery, "\n")
        execut
        cursor.execute(insertQuery)

def main():
    readAndStoreJsonObject(ipJson)

if __name__ == "__main__":
    main()

