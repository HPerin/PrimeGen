//
//  ViewController.swift
//  Client-Swift
//
//  Created by Caio Lunardi on 24/06/17.
//  Copyright © 2017 Caio Lunardi. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    @IBOutlet weak var textView: UITextView!
    
    @IBOutlet weak var switchControl: UISwitch!
    
    @IBOutlet weak var ipTextField: UITextField!
    
    @IBOutlet weak var portTextField: UITextField!
    
    var generator : Generator!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        self.textView.text = "Press button to start running"
        
        DispatchQueue.global().async {
            self.worker()
//            self.worker()
        }
    }
    
    func worker() {
        while true {
            if self.switchControl.isOn {
                if (self.generator == nil) {
                    if let generator = Generator.init(address:self.ipTextField.text!, port:Int32(self.portTextField.text!)!) {
                        self.generator = generator
                    } else {
                        DispatchQueue.main.async {
                            self.switchControl.setOn(false, animated: true)
                        }
                    }
                }
                if self.switchControl.isOn {
                    let primes = self.generator.runOnce()
                    
                    DispatchQueue.main.async {
                        if (primes != nil) {
                            self.textView.text.append(String.init(format: "\nProcessed block %d - %d", primes!.first!, primes!.last!))
                        } else {
                            self.textView.text.append("\nBlock process failed")
                        }
                    }
                }
            } else {
                Thread.sleep(forTimeInterval: 0.5)
            }
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    @IBAction func switchChanged(_ sender: Any) {
        self.generator = nil
        if self.switchControl.isOn {
            if (self.ipTextField.text == nil) || (self.portTextField.text == nil) {
                self.switchControl.setOn(false, animated: true)
            }
            if Int(self.portTextField.text!) == nil {
                self.switchControl.setOn(false, animated: true)
            }
        }
    }

}

