# Flask JSON Storage with Database Persistence API

This is a simple Flask application that provides a REST API for storing, retrieving, updating, and deleting JSON objects in an MongoDb database.

## Requirements

- Python 3.x
- `pip` (Python package installer)

## Setup

### Automated Setup (Windows)

1. Clone the repository or download the project files to your local machine.
2. Open Command Prompt and navigate to the project directory.
3. Run the batch file to set up the environment, install dependencies, provide the database URL and Credentials in config.json, and start the Flask server:

    ```cmd
    setup.bat
    ```

### Manual Setup (Cross-Platform)

1. Clone the repository or download the project files to your local machine.
2. Create and activate a virtual environment:

    ```sh
    python -m venv venv
    ```

   - On Windows:

     ```sh
     venv\Scripts\activate
     ```

   - On macOS/Linux:

     ```sh
     source venv/bin/activate
     ```

3. Install Flask:

    ```sh
    pip install Flask
    ```

4. Initialize the database:

    ```sh
    Provide your MongoDb URL and Credentials in config.json
    END
    ```

5. Set the `FLASK_APP` environment variable and start the Flask server:

   - On Windows:

     ```cmd
     set FLASK_APP=app.py
     flask run --host=0.0.0.0
     ```

   - On macOS/Linux:

     ```sh
     export FLASK_APP=app.py
     flask run --host=0.0.0.0
     ```

## API Endpoints

### Store Data

- **URL**: `/data`
- **Method**: `POST`
- **Content-Type**: `application/json`
- **Request Body**: JSON object to be stored
- **Response**: 

    ```json
    {
        "message": "Data stored successfully"
    }
    ```

- **Example**:

    ```sh
    curl -X POST http://127.0.0.1:5000/data -H "Content-Type: application/json" -d "{\"one\":\"1.232.123\",\"two\":\"test\",\"three\":1242,\"four\":\"2,4\",\"five\":\"test\",\"six\":\"test\",\"seven\":\"test\"}"
    ```

### Retrieve Data

- **URL**: `/data/<id>`
- **Method**: `GET`
- **Response**:

    ```json
    {
        "id": <id>,
        "data": "<json_data>"
    }
    ```

- **Example**:

    ```sh
    curl -X GET http://127.0.0.1:5000/data/"668a3b0c6acda8b2a7206ea6"
    ```

### Update Data

- **URL**: `/data/<id>`
- **Method**: `PUT`
- **Content-Type**: `application/json`
- **Request Body**: JSON object to update
- **Response**:

    ```json
    {
        "message": "Data updated successfully"
    }
    ```

- **Example**:

    ```sh
    curl -X PUT http://127.0.0.1:5000/data/"668a42a360026f7dd4e37281" -H "Content-Type: application/json" -d "{\"key\": \"new_value2\"}"
    ```

### Delete Data

- **URL**: `/data/<id>`
- **Method**: `DELETE`
- **Response**:

    ```json
    {
        "message": "Data deleted successfully"
    }
    ```

- **Example**:

    ```sh
    curl -X DELETE http://127.0.0.1:5000/data/"668a42a360026f7dd4e37281"
    ```

## License

This project is licensed under the MIT License.
