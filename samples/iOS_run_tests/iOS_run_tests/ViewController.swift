/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

import UIKit

class ViewController: UIViewController {
    
    @IBOutlet weak var libraryVersionLabel: UILabel!
    @IBOutlet weak var testsResultsLabel: UILabel!

    override func viewDidLoad() {
        super.viewDidLoad()
        
        let testResults = run_tests(1, UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>.allocate(capacity: 1))
        
        testsResultsLabel.text = testResults == 0 ? "Ok" : "Not Ok"
        
        let version = UnsafeMutablePointer<UnsafePointer<Int8>?>.allocate(capacity: 1)
        
        
        defer { version.deallocate(capacity: 1) }
        let error = make_version_string(version)
        
        if error != nil {
            let errrString = returnErrorString(opaquePointer: error!, mask: "make_version_string")
            libraryVersionLabel.text = errrString
        } else {
            let versionString = String(cString: version.pointee!)
            libraryVersionLabel.text = versionString
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    func returnErrorString(opaquePointer: OpaquePointer, mask: String) -> String {
        let pointer = UnsafeMutablePointer<MultyError>(opaquePointer)
        let errorString = String(cString: pointer.pointee.message)
        
        print("\(mask): \(errorString))")
        
        defer {
            pointer.deallocate(capacity: 1)
        }
        
        return errorString
    }
}

