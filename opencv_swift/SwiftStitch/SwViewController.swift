//
//  SwViewController.swift
//  CVOpenStitch
//
//  Created by Foundry on 04/06/2014.
//  Copyright (c) 2014 Foundry. All rights reserved.
//

import UIKit
import MobileCoreServices

class SwViewController: UIViewController,UIImagePickerControllerDelegate,UINavigationControllerDelegate {
    
    var imageView = UIImageView()
    
    @IBOutlet weak var dog1: UIImageView!
    
    @IBOutlet weak var dog2: UIImageView!
    
   
    @IBOutlet weak var classname1: UILabel!
    
    @IBOutlet weak var classname2: UILabel!
    
    
       @IBAction func takePhotos() {
        if UIImagePickerController.isSourceTypeAvailable(.Camera){
            let picker = UIImagePickerController()
            picker.sourceType = .Camera
            picker.mediaTypes = [kUTTypeImage as String]
            picker.delegate = self
            picker.allowsEditing = true
            presentViewController(picker, animated: true, completion: nil)
            
        }

        
    }
    func imagePickerController(picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [String : AnyObject])
    {
        var image = info[UIImagePickerControllerEditedImage] as? UIImage
        if image == nil{
            image = info[UIImagePickerControllerOriginalImage] as? UIImage
        }
        imageView.image = image
        dismissViewControllerAnimated(true, completion: nil)
        let dogbreed = self.classpredict(image!).characters.split{$0 == " "}
        
        classname1.text = dog_breed[String(dogbreed[0])]
        dog1.image = UIImage(named: classname1.text!+".jpg")
        classname2.text = dog_breed[String(dogbreed[1])]
        dog2.image = UIImage(named: classname2.text!+".jpg")
        
        
    }
    func imagePickerControllerDidCancel(picker: UIImagePickerController) {
        dismissViewControllerAnimated(true, completion: nil)
    }
    
    func classpredict(image: UIImage)-> String{
       
        let result:String = CVWrapper.predict(image)
        return result;
    }
}
