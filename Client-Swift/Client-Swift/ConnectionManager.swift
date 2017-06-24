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
    
    init(address:String, port:Int32) {
        // Create a socket connect to www.apple.com and port at 80
        self.client = TCPClient.init(address: address, port: port)
        
        switch self.client.connect(timeout: 10) {
        case .success:
            // Connection successful 🎉
            print("Connected.")
        case .failure(let error):
            // 💩
            print("Failure. Error: ", error)
        }
    }
    
    func getBlock() -> [String:Int] {
        var block = Dictionary<String, Int>.init()
        block["START"] = 2
        block["END"] = 1000
        return block
    }
    
    func sendResult(block:[String:Int], primes:[Int]) {
        var msg = Dictionary<String, Any>.init()
        msg["type"] = "BLOCK_RESULT"
        msg["id"] = block["id"]
        msg["block_start"] = block["block_start"]
        msg["block_end"] = block["block_end"]
        msg["data"] = primes
        
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
    }
    
    func send(data:Data, separator:Character) -> Result {
        var stringData = String.init(data: data, encoding: .utf8)
        stringData?.append(separator)
        return self.client.send(string: stringData!)
    }
    
    func receiveUntil(separator:Character) -> Data {
        var receivedString = String.init()
        var receivedBytes : [Byte]! = nil
        repeat {
            receivedBytes = self.client.read(1, timeout: 10)
            if (receivedBytes?.count)! < 1 {
                print("Receive failed.")
                return Data.init()
            }
            if (receivedBytes![0] != Byte.init(separator.hashValue)) {
                receivedString.append(Character.init(String.init(receivedBytes![0])))
            }
        } while(receivedBytes![0] != Byte.init(separator.hashValue))
        return receivedString.data(using: .utf8)!
    }
}


