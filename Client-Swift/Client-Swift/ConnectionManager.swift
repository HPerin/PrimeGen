//
//  ConnectionManager.swift
//  Client-Swift
//
//  Created by Caio Lunardi on 24/06/17.
//  Copyright © 2017 Caio Lunardi. All rights reserved.
//

import Foundation
import SwiftSocket

class ConnectionManager {
    var client : TCPClient
    
    init?(address:String, port:Int32) {
        // Create a socket connect to www.apple.com and port at 80
        self.client = TCPClient.init(address: address, port: port)
        
        switch self.client.connect(timeout: 10) {
        case .success:
            // Connection successful 🎉
            print("Connected.")
        case .failure(let error):
            // 💩
            print("Failure. Error: ", error)
            return nil
        }
    }
    
    deinit {
        self.client.close()
    }
    
    func getBlock() -> [String:Int] {
        var msg = Dictionary<String, Any>.init()
        msg["type"] = "BLOCK_REQUEST"
        
        do {
            let json = try JSONSerialization.data(withJSONObject: msg, options: .prettyPrinted) as Data
            
            switch self.send(data: json, separator: "$") {
            case .success:
                print("Send success")
            case .failure(let error):
                print("Send failure: ", error)
            }
        } catch {
            print("Error generating json.", error)
        }
        
        let response = receiveUntil(separator: "$")
        
        var block = Dictionary<String, Int>.init()
        
        do {
            let obj = try JSONSerialization.jsonObject(with: response, options: JSONSerialization.ReadingOptions.allowFragments)
            
            if let dict = obj as? Dictionary<String, String> {
                for key in dict.keys {
                    block[key] = Int(dict[key]!)
                }
            } else {
                print("Could not convert")
            }
        } catch {
            print("Error receiving json.", error)
        }
        
        print(block)
        
        return block
    }
    
    func sendResult(block:[String:Int], primes:[Int]) -> Bool {
        var msg = Dictionary<String, Any>.init()
        msg["type"] = "BLOCK_RESULT"
        msg["id"] = String.init(format: "%d", block["id"]!)
        msg["block_start"] = String.init(format: "%d", block["block_start"]!)
        msg["block_end"] = String.init(format: "%d", block["block_end"]!)
        msg["data"] = primes
        
        do {
            let json = try JSONSerialization.data(withJSONObject: msg, options: .prettyPrinted) as Data
            
            switch self.send(data: json, separator: "$") {
            case .success:
//                print("Send success")
                break
            case .failure(let error):
                print("Send failure: ", error)
                return false
            }
        } catch {
            print("Error generating json.", error)
            return false
        }
        
        let response = receiveUntil(separator: "$")
        
        do {
            let obj = try JSONSerialization.jsonObject(with: response, options: JSONSerialization.ReadingOptions.allowFragments)
            
            if let dict = obj as? Dictionary<String, Any> {
                if let valid = dict["valid"] as? Bool {
                    if valid {
//                        print("Valid")
                        return true
                    } else {
                        print("Invalid")
                        return false
                    }
                } else {
                    print("Failed")
                    return false
                }
            } else {
                print("Could not convert")
                return false
            }
        } catch {
            print("Error receiving json.", error)
            return false
        }
    }
    
    func send(data:Data, separator:Character) -> Result {
        var stringData = String.init(data: data, encoding: .utf8)
        stringData?.append(separator)
        return self.client.send(string: stringData!)
    }
    
    func receiveUntil(separator:Character) -> Data {
//        var receivedString = String.init()
        var receivedBytes : [Byte]! = nil
//        repeat {
//            receivedBytes = self.client.read(1, timeout: 10)
//            if (receivedBytes != nil) == false {
//                print("Receive failed.")
//                return Data.init()
//            }
//            if (Int(receivedBytes!.first!) != separator.hashValue) {
//                receivedString.append(String.init(receivedBytes!.first!))
//            }
//        } while(Int(receivedBytes!.first!) != separator.hashValue)
        receivedBytes = self.client.read(1024, timeout: 10)
        if (receivedBytes != nil) == false {
            print("Receive failed.")
            return Data.init()
        }
        var receivedString = String.init(cString: receivedBytes)
        receivedString = receivedString.substring(to: (receivedString.rangeOfCharacter(from: CharacterSet.init(charactersIn: "$"))?.lowerBound)!)
        print(receivedString)
    
        return receivedString.data(using: .utf8)!
    }
}


