//
//  Generator.swift
//  Client-Swift
//
//  Created by Caio Lunardi on 24/06/17.
//  Copyright © 2017 Caio Lunardi. All rights reserved.
//

import Foundation

class Generator {
    let connectionManager : ConnectionManager
    
    init() {
        self.connectionManager = ConnectionManager.init(address: "127.0.0.1", port: 5000)
    }
    
    func runOnce() -> [Int] {
        let block = self.connectionManager.getBlock()
        
        let primes = calculate(block: block)
        
        self.connectionManager.sendResult(block: block, primes: primes)
        
        return primes
    }
    
    private func calculate(block:[String:Int]) -> [Int] {
        var primes = Array<Int>.init()
        
        if let start=block["block_start"], let end=block["block_end"] {
            for i in start...end {
                if isPrime(i) {
                    primes.append(i)
                }
            }
        }
        return primes
    }
    
    private func isPrime(_ val:Int) -> Bool {
        if val % 2 == 0 {
            return false
        }
        
        var div = 3
        let finalDiv = Int(val/2)
        while div <= finalDiv {
            if val % div == 0 {
                return false
            }
            div += 2
        }
        return true
    }
}
