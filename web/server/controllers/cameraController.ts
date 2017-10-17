import { NextFunction, Request, Response } from "express";
import { Cameras } from "./../models/cameras";
import { login } from "./../models/login";
import { ServerLogger } from "./../serverlog/logger";
import { ValidateCamerasRequest } from "./validate/validateCamerasRequest";

const cameras = new Cameras();
const validate = new ValidateCamerasRequest();
const logger = new ServerLogger();

export class CameraController {
    constructor() {
        // empty
    }

    public getCameras(req: Request, res: Response, next: NextFunction) {
        logger.addInfo("Called get all cameras");
        cameras.getCameras(req, res);
    }

    public getCameraById(req: Request, res: Response, next: NextFunction) {
        if (validate.checkGetCameraById(req, res, next)) {
            logger.addInfo("Called get camera by id with id = " + req.params.id);
            cameras.getCameraById(req, res);
        }
    }

    public getCamerasWithRange(req: Request, res: Response, next: NextFunction) {
        res.send("Get Camera with range");
    }

    public deleteCameraById(req: Request, res: Response, next: NextFunction) {
        if (validate.checkDeleteCameraById(req, res, next)) {
            logger.addInfo("Called delete camera by id with id = " + req.params.id);
            login.isLogin(req, res, cameras.deleteCameraById);
        }
    }

    public updateCameraById(req: Request, res: Response, next: NextFunction) {
        if (validate.checkUpdateCameraById(req, res, next)) {
            logger.addInfo("Called edit camera by id with id = " + req.params.id);
            login.isLogin(req, res, cameras.updateCameraById);
        }
    }

}

const cameraController = new CameraController();
export default cameraController;
