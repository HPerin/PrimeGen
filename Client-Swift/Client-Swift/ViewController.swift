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
    
    var generator : Generator!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        self.textView.text = "Press button to start running"
        self.generator = Generator.init()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    @IBAction func runPressed(_ sender: Any) {
        let primes = self.generator.runOnce()
        
        self.textView.text = primes.description
    }

}

