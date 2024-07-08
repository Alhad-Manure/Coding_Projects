from flask import Flask, request, jsonify
import json
import databaseHandler

###### Flask Application ######
# Creating Flask application
app = Flask(__name__)

# POST request handling
# Throws "400 Bad Request" if Invalid json is passed
@app.route('/data', methods=['POST'])
def store_data():
    if request.is_json:
        data = request.get_json()
        
        mongodbObj = databaseHandler.databaseHandler()
        mongodbObj.insertJsonObjectToDb(data)
        
        return jsonify({"message": "Data stored successfully"}), 201
    else:
        return jsonify({"error": "Request must be JSON"}), 400 


# GET request handling
# Throws "404 Data not found" if Invalid ID is passed.
@app.route('/data/<ipId>', methods=['GET'])
def get_data(ipId):
    mongodbObj = databaseHandler.databaseHandler()
    res = mongodbObj.getObjectWithGivenId(id = ipId )
    
    if res:
        return jsonify(res), 200
    else:
        return jsonify({"error": "Data not found"}), 404


# PUT request handling
# Throws "404 Data not found" if Invalid ID is passed.
@app.route('/data/<ipId>', methods=['PUT'])
def update_data(ipId):

    if request.is_json:
        data = request.get_json()

        mongodbObj = databaseHandler.databaseHandler()
        json_str = str(data)
        res = mongodbObj.updateTheJsonObjOfId( id=ipId, dataToUpdate=data)
        
        if res > 0:
            return jsonify({"message": "Data updated successfully"}), 200
        else:
            return jsonify({"error": "Data not found"}), 404
    else:
        return jsonify({"error": "Request must be JSON"}), 400


# DELETE request handling
# Throws "404 Data not found" if Invalid ID is passed.
@app.route('/data/<ipId>', methods=['DELETE'])
def delete_data(ipId):

    mongodbObj = databaseHandler.databaseHandler()
    result = mongodbObj.deleteTheJsonObjOfId( id=ipId )    

    if result > 0:
        return jsonify({"message": "Data deleted successfully"}), 200
    else:
        return jsonify({"error": "Data not found"}), 404

def main():
    app.run(debug=True)

if __name__ == "__main__":
    main()