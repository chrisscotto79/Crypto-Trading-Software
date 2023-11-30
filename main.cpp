// Christopher Scotto
// Create a small software application that allows the user to connect to Alpaca API and buy/sell Bitcoin.

#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <curl/curl.h>

using namespace std;
using json = nlohmann::json;

// baseurl to fetch the data and send in the request
const string baseUrl = "https://paper-api.alpaca.markets";

// API key for alpaca api, API secret key under the same account
const string apiKey = "PKGUY9U18KB7D9BP0PIU";
const string secretKey = "vK4fEOetkVch5T171ZYsJ6KiIEpl5Wub9PJaGj71";
class InputChecker{
public:
    // Set a function to check if the input is an integer, if it isn't then it throws an error and makes the user re-enter an input.
    int checkInputInt(int input){
        // A while loop with the fail function that returns true if the input throws an error
        while(cin.fail()) {
            //prints out an error message asking to type in a new int.
            cout << "Error: Invalid input, Please enter in a integer." << endl;
            // Clears out the error message
            cin.clear();
            // After the error message is cleared, it removes all the characters in the varible with the ignore function.
            cin.ignore(256,'\n');
            // A new input is asked.
            cin >> input;
        }
        return 0;
    }

// Set a function to check if the input is an integer, if it isn't then it throws an error and makes the user re-enter an input.
    int checkInputString(string input){

        // A while loop with the fail function that returns true if the input throws an error
        while(cin.fail()) {
            //prints out an error message asking to type in a new int.
            cout << "Error: Invalid input, Please enter in a integer." << endl;
            // Clears out the error message
            cin.clear();
            // After the error message is cleared, then it removes all the characters in the variable with the ignore function.
            cin.ignore(256,'\n');
            // A new input is asked.
            cin >> input;
        }
        return 0;
    }
};




// Callback function to handle the alpaca API response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    // Calculate the total size of the received data
    size_t totalSize = size * nmemb;

    // Append the received data to the output string
    // 'contents' is a pointer to the received data
    // 'totalSize' is the size of the data block
    output->append((char*)contents, totalSize);

    // Return the total size of the received data
    return totalSize;
}


// CURL function with the curl library to handle the request of Alpaca to send POST and GET functions.
CURL *createCurlHandle() {
    // CURL documentation - "handle is used as input to other functions in the easy interface"
    CURL* curl = curl_easy_init();

    // If loop to display if the curl is set to true.
    if (curl) {
        // Set API base URL
        curl_easy_setopt(curl, CURLOPT_URL, baseUrl.c_str());

        // Add headers and authentication
        struct curl_slist* headers = nullptr;
        // API key and secret declared and being called within the headers, so it is not called multiple times through the code.
        headers = curl_slist_append(headers, ("APCA-API-KEY-ID: " + apiKey).c_str());
        headers = curl_slist_append(headers, ("APCA-API-SECRET-KEY: " + secretKey).c_str());

        // CURL documentation - "curl_easy_setopt is used to tell libcurl how to behave."
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Return the variable when the function is called.
        return curl;
    }

    // if curl is false, then return null or empty.
    return nullptr;
}


double getBitcoinPrice() {
    CURL* curl = curl_easy_init();
    if (curl) {
        string url = "https://data.alpaca.markets/v1beta3/crypto/us/latest/orderbooks?symbols=BTC/USD";

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Apca-Api-Key-Id: " + apiKey).c_str());
        headers = curl_slist_append(headers, ("Apca-Api-Secret-Key: " + secretKey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        string responseString;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Failed to fetch data: " << curl_easy_strerror(res) << endl;
            curl_easy_cleanup(curl);
            return 0.0;
        }

        // Parse the JSON response
        json responseJson;
        try {
            responseJson = json::parse(responseString);
        } catch (const json::parse_error& e) {
            cerr << "JSON parsing error: " << e.what() << endl;
            cerr << "Raw response: " << responseString << endl;
            curl_easy_cleanup(curl);
            return 0.0;
        }

        // Check if the "BTC/USD" entry exists in the orderbooks
        if (responseJson.contains("orderbooks") && responseJson["orderbooks"].contains("BTC/USD")) {
            // Accessing the "p" value (price) of the latest ask and bid for BTC/USD
            double btcUsdPriceAsk = responseJson["orderbooks"]["BTC/USD"]["a"][0]["p"].get<double>();
            double btcUsdPriceBid = responseJson["orderbooks"]["BTC/USD"]["b"][0]["p"].get<double>();

            double btcUsdAvgPrice = (btcUsdPriceAsk + btcUsdPriceBid) / 2;

            curl_easy_cleanup(curl);
            return btcUsdAvgPrice; // You can choose to return ask, bid, or a calculated average
        } else {
            cerr << "BTC/USD data not found in the response." << endl;
            cerr << "Raw response: " << responseString << endl;
            curl_easy_cleanup(curl);
            return 0.0;
        }
    } else {
        cerr << "Failed to initialize cURL." << endl;
        return 0.0;
    }
}


double getResField(string field){

        // Set curl for the function createCurlHandle.
        CURL* curl = createCurlHandle();
        // An if statement that sends a message to the terminal that the curl returned false.
        if (!curl) {
            // returning false message
            cout << "Failed to create a cURL handle." << endl;
            return 0;
        }

        // Set the specific endpoint for account information
        const char* endpoint = "/v2/account";
        curl_easy_setopt(curl, CURLOPT_URL, (baseUrl + endpoint).c_str());

        // Create a variable to store the JSON response
        string responseBody;

        // Set a callback function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

        // Perform the GET request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Handle the API response
            nlohmann::json jsonParsedResponse;

            // Parse the response data as JSON
            if (!responseBody.empty()) {
                jsonParsedResponse = nlohmann::json::parse(responseBody);

                    // if loop to see if the fields is shown in the json response, and if it is, it is true.
                    if (jsonParsedResponse.find(field) != jsonParsedResponse.end() &&
                        !jsonParsedResponse[field].is_null()) {

                        // Extract the field value
                        string parsedMessage = jsonParsedResponse[field];

                        cout << parsedMessage << endl;

                    }

            } else {
                cout << "Empty response received." << endl;
            }
        }
    curl_easy_cleanup(curl);
    return 0.0;
}


// To view the balance and account information of the current user's account.
void getAccountBalance() {
    // Set curl for the function createCurlHandle.
    CURL* curl = createCurlHandle();
    // An if statement that sends a message to the terminal that the curl returned false.
    if (!curl) {
        // returning false message
        cout << "Failed to create a cURL handle." << endl;
        return;
    }

    // Set the specific endpoint for account information
    const char* endpoint = "/v2/account";
    curl_easy_setopt(curl, CURLOPT_URL, (baseUrl + endpoint).c_str());

    // Create a variable to store the JSON response
    string responseBody;

    // Set a callback function to capture the response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

    // Perform the GET request
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        // Handle the API response
        nlohmann::json jsonParsedResponse;

        // Parse the response data as JSON
        if (!responseBody.empty()) {
            jsonParsedResponse = nlohmann::json::parse(responseBody);


            // array to define the field names to extract
            string fields[3] = {"buying_power", "equity", "position_market_value"};

            cout << "==================================" << endl;
            cout << "===== Account Response Data ======" << endl;
            // for loop that goes through the number array and outputs the fields response.
            for (const auto & field : fields) {
                // if loop to see if the fields is shown in the json response, and if it is, it is true.
                if (jsonParsedResponse.find(field) != jsonParsedResponse.end() &&
                    !jsonParsedResponse[field].is_null()) {

                    // Extract the field value
                    string parsedMessage = jsonParsedResponse[field];

                    // Print the field name and value
                    cout << "    - " << field << ": " << parsedMessage << endl;
                }
            }
            cout << "    - " << "Bitcoin Price" << ": $" << getBitcoinPrice() << endl;
            cout << "==================================" << endl;
        } else {
            cout << "Empty response received." << endl;
        }


    }

    curl_easy_cleanup(curl);
}


// Buy or sell amount for Bitcoin
void placeOrder(double amount, string buyOrSell) {
    CURL* curl = createCurlHandle();
    if (!curl) {
        cout << "Failed to create a cURL handle." << endl;
        return;
    }

    const char* endpoint = "/v2/orders";
    curl_easy_setopt(curl, CURLOPT_URL, (baseUrl + endpoint).c_str());

    // Create JSON payload for the buy order
    json orderPayload = {
            {"symbol", "BTC/USD"},     // Change the symbol to BTC/USD
            {"qty", amount},           // Quantity to buy or sell (specified by the user)
            {"side", buyOrSell},       // Buy or sell order
            {"type", "market"},        // Market order type
            {"time_in_force", "gtc"}   // Good 'til canceled
    };

    // Convert JSON payload to a string
    string orderPayloadStr = orderPayload.dump();

    // Set the payload for the POST request
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, orderPayloadStr.c_str());

    // Create a variable to store the JSON response
    string responseBody;

    // Set a callback function to capture the response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

    // Perform the POST request
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        // Handle specific error codes
        if (res == CURLE_HTTP_RETURNED_ERROR) {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            if (httpCode == 400) {
                // Handle 400 error (Bad Request)
                cout << "Error: Bad Request - Check your order parameters." << endl;
            } else if (httpCode == 403) {
                // Handle 403 error (Forbidden)
                cout << "Error: Forbidden - Crypto orders not allowed for account." << endl;
                // Add further instructions or actions as needed
            } else {
                // Handle other HTTP errors
                cout << "HTTP Error: " << httpCode << endl;
            }
        }
    } else {
        cout << "Bitcoin order placed." << endl;

        // Parse the response for the "message" field
        try {
            // Parse the response data as JSON
            nlohmann::json jsonParsedResponse = nlohmann::json::parse(responseBody);
            string fields[5] = {"message","status", "side","symbol", "submitted_at", };

            for (int i = 0; i < 3; i++) {
                if (jsonParsedResponse.find(fields[i]) != jsonParsedResponse.end() &&
                    !jsonParsedResponse[fields[i]].is_null()) {

                    // Extract the "buying_power" field
                    string parsedMessage = jsonParsedResponse[fields[i]];


                    // Print only the "buying_power" value
                    cout << fields[i] << ": " << parsedMessage << endl;
                }else {
                    cout << fields[i] << ": null or empty" << endl;
                }
            }
            cout << "Please check with your dashboard for further information about your order.\n" << endl;
            getAccountBalance();

        } catch (const nlohmann::json::parse_error& e) {
            cerr << "JSON parsing error: " << e.what() << endl;
            cerr << "Raw response: " << responseBody << endl;
        }
    }

}


int main() {
    string name;
    cout << "----------Welcome to Bitcoin Trading Software----------" << endl;
    cout << "---------------By Christopher Scotto---------------" << endl;

    string existingUser;
    InputChecker checkString;
    cout << "Are you an existing user? (T/F)" << endl;
    cin >> existingUser;
    checkString.checkInputString(existingUser); // Validate the input as a string


    json userData;

    if (existingUser == "T" || existingUser == "t") {
        string fileName = "UserData.json";
        ifstream file(fileName);

        if (file.is_open()) {
            // Read user data from the JSON file if it exists
            file >> userData;
            cout << "Welcome back, existing user!" << endl;
            name = userData["name"]; // Read the user's name from userData
            cout << "User's name: " << name << endl;

            // You can access other user data fields similarly
        } else {
            cout << "No user data currently found...\nMaking a new database." << endl;
            // Initialize or prompt for user data and save it to the JSON file
            cout << "Enter your name: ";
            cin >> name;
            userData["name"] = name;
            // Add more user data as needed

            // Save the user data to the JSON file
            ofstream outFile(fileName);
            outFile << userData.dump(4); // Use pretty print format with 4 spaces
            outFile.close();
        }
        file.close();
    } else {
        cout << "Making a new database." << endl;
        cout << "Enter your name: ";
        cin >> name;
        userData["name"] = name;
        ofstream outFile("UserData.json");
        outFile << userData.dump(4); // Use pretty print format with 4 spaces
        outFile.close();
    }


    int choice = 0;
    while (choice != 4) {
        cout << "Menu:" << endl;
        cout << "*---BTC trading at ~$" << getBitcoinPrice() << "---*" << endl;

        cout << "|  1. User's Account Balance   |" << endl;
        cout << "|  2. Buy Bitcoin              |" << endl;
        cout << "|  3. Sell Bitcoin             |" << endl;
        cout << "|  4. Exit                     |"  << endl;
        cout << "________________________________" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        InputChecker checkInt;

        checkInt.checkInputInt(choice);
            switch (choice) {
                case 1:
                    cout << "You've selected to view your account.\n" << endl;
                    try {
                        cout << "Getting user's balance..." << endl;
                        getAccountBalance();

                        string backButton;
                        cout << "Would you like to go back to the menu? (T/F)" << endl;
                        cin >> backButton;

                        if(backButton == "T" or backButton == "t"){
                            break;
                        }else{
                            return 0;
                        }

                    } catch (const exception& e) {
                        cout << "An error occurred: " << e.what() << endl;
                        string viewError;
                        cout << "To continue or retry, press any key. To view the error message, press 'V': ";
                        cin >> viewError;

                        if (viewError == "V" || viewError == "v") {
                            cout << e.what() << endl;
                        } else {
                            cout << "Bringing you back to the menu..." << endl;
                        }
                    }
                    break;
                case 2:
                    cout << "You've selected to Buy Bitcoin..." << endl;
                    cout << "The current Price of Bitcoin: " << getBitcoinPrice() << endl;
                    cout << "Current buying power: " << getResField("buying_power") << endl;

                    try {
                        bool purchaseAmount = false;
                        double amount;

                        while (!purchaseAmount) {
                            cout << "How much would you like to purchase in dollars: ";
                            cin >> amount;

                            if (cin.fail()) {
                                cout << "Invalid input. Please enter a valid numeric value." << endl;
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                continue;
                            }

                            double bitcoinPrice = getBitcoinPrice();
                            if (bitcoinPrice <= 0) {
                                cout << "Failed to retrieve Bitcoin price. Please try again later." << endl;
                                break;
                            }

                            double convert = (amount / bitcoinPrice);

                            cout << "\nPurchase Amount (in dollars): " << amount << endl;
                            cout << "Equivalent Bitcoin amount: " << convert << endl;

                            cout << "Is the above amount correct? 'YES' or 'NO'" << endl;
                            string validate;
                            cin >> validate;

                            if (validate == "YES" || validate == "yes") {
                                cout << "Sending Order..." << endl;
                                placeOrder(convert, "buy");
                                purchaseAmount = true;

                            } else if (validate == "NO" || validate == "no") {
                                cout << "User inputted 'NO', sending user back to the amount wanted..." << endl;
                            } else {
                                cout << "Invalid input. Please enter 'YES' or 'NO'." << endl;
                            }
                        }
                    } catch (const exception& e) {
                        cout << "An error occurred: " << e.what() << endl;
                        string viewError;
                        cout << "To continue or retry, press any key. To view the error message, press 'V': ";
                        cin >> viewError;

                        if (viewError == "V" || viewError == "v") {
                            cout << e.what() << endl;
                        } else {
                            cout << "Bringing you back to the menu..." << endl;
                        }
                    }
                    break;

                case 3:
                    cout << "You've selected to Sell bitcoin..." << endl;
                    try {
                        double bitcoinPrice = getBitcoinPrice();
                        double amount;
                        getAccountBalance();

                        cout << "You have $" << getResField("position_market_value") << " invested in bitcoin." << endl;

                        cout << "In dollars, how much would you like to sell? ";
                        cin >> amount;

                        if (cin.fail() || amount <= 0) {
                            throw invalid_argument("Invalid amount entered. Please enter a valid positive number.");
                        }

                        // Calculate the equivalent Bitcoin to sell based on the amount entered
                        double convert = (amount / bitcoinPrice);

                        // Display the Bitcoin amount the user will sell
                        cout << "You are selling $" << amount << " for approximately " << convert << " Bitcoin." << endl;

                        // Place the sell order
                        placeOrder(convert, "sell");
                    } catch (const exception &e) {
                        cout << "An error occurred: " << e.what() << endl;
                        string viewError;
                        cout << "To continue or retry, press any key. To view the error message, press 'E': ";
                        cin >> viewError;

                        if (viewError == "E" || viewError == "e") {
                            cout << e.what() << endl;
                        } else {
                            cout << "Bringing you back to the menu..." << endl;
                        }
                    }
                    break;


                case 4:
                    cout << "Exiting the program." << endl;
                    return 0;
                default:
                    cout << "Invalid choice. Please select a valid option." << endl;
                    continue;

            }
    }
}
