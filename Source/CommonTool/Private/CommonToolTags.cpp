// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonToolTags.h"

namespace CommonToolTags
{
	//System state
	UE_DEFINE_GAMEPLAY_TAG(State_Main, "State.Main")
	UE_DEFINE_GAMEPLAY_TAG(State_MainMenu, "State.MainMenu")
	UE_DEFINE_GAMEPLAY_TAG(State_Simulate_Maintenance, "State.Simulate.Maintenance")
	UE_DEFINE_GAMEPLAY_TAG(State_Edit, "State.Edit")
	UE_DEFINE_GAMEPLAY_TAG(State_Inspection, "State.Inspection")
	UE_DEFINE_GAMEPLAY_TAG(State_WeatherSimulation, "State.WeatherSimulation")
	UE_DEFINE_GAMEPLAY_TAG(State_DigitalArchives, "State.DigitalArchives")
	UE_DEFINE_GAMEPLAY_TAG(State_Roam, "State.Roam")
	UE_DEFINE_GAMEPLAY_TAG(State_Highlights, "State.Highlights")
	UE_DEFINE_GAMEPLAY_TAG(State_Null, "State.Null")

	UE_DEFINE_GAMEPLAY_TAG(DeviceState_Ledger, "DeviceState.Ledger")
	UE_DEFINE_GAMEPLAY_TAG(DeviceState_OperatingData, "DeviceState.OperatingData")
	UE_DEFINE_GAMEPLAY_TAG(DeviceState_History, "DeviceState.Histor")
	UE_DEFINE_GAMEPLAY_TAG(DeviceState_Document, "DeviceState.Document")
	UE_DEFINE_GAMEPLAY_TAG(DeviceState_Null, "DeviceState.Null")
	UE_DEFINE_GAMEPLAY_TAG(DeviceState_Disassemble, "DeviceState.Disassemble")
	
	UE_DEFINE_GAMEPLAY_TAG(Actor_Type_Null, "Actor.Type.Null")
	UE_DEFINE_GAMEPLAY_TAG(Actor_Type_Device, "Actor.Type.Device")
	UE_DEFINE_GAMEPLAY_TAG(Actor_Type_Build, "Actor.Type.Build")

	UE_DEFINE_GAMEPLAY_TAG(State_DigitalArchives_Design, "State.DigitalArchives.Design")
	UE_DEFINE_GAMEPLAY_TAG(State_DigitalArchives_Build, "State.DigitalArchives.Build")
	UE_DEFINE_GAMEPLAY_TAG(State_DigitalArchives_CheckAndAccept, "State.DigitalArchives.CheckAndAccept")
	UE_DEFINE_GAMEPLAY_TAG(State_DigitalArchives_Operation, "State.DigitalArchives.Operation")
	
	UE_DEFINE_GAMEPLAY_TAG(DialogState_Null, "DialogState.Null")
	UE_DEFINE_GAMEPLAY_TAG(DialogState_Set, "DialogState.Set")
	UE_DEFINE_GAMEPLAY_TAG(DialogState_Weather, "DialogState.Weather")
	UE_DEFINE_GAMEPLAY_TAG(DialogState_EditorOption, "DialogState.EditorOption")
	UE_DEFINE_GAMEPLAY_TAG(DialogState_EditorAttribute, "DialogState.EditorAttribute")
	UE_DEFINE_GAMEPLAY_TAG(DialogState_MarkType, "DialogState.MarkType")
	UE_DEFINE_GAMEPLAY_TAG(DialogState_Introduction, "DialogState.Introduction")
	UE_DEFINE_GAMEPLAY_TAG(DialogState_Measurement, "DialogState.Measurement")
	UE_DEFINE_GAMEPLAY_TAG(DialogState_WiringDiagram, "DialogState.WiringDiagram")
	
	//User操作状态
	UE_DEFINE_GAMEPLAY_TAG(UserState_Panorama, "UserState.Panorama"); //全景
	UE_DEFINE_GAMEPLAY_TAG(UserState_FirstPerson, "UserState.FirstPerson"); //第一人称
	UE_DEFINE_GAMEPLAY_TAG(UserState_ThirdPerson, "UserState.ThirdPerson"); //第三人称
	UE_DEFINE_GAMEPLAY_TAG(UserState_Focus, "UserState.Focus"); //聚焦
	UE_DEFINE_GAMEPLAY_TAG(UserState_Free, "UserState.Free"); //自由
	UE_DEFINE_GAMEPLAY_TAG(UserState_Null, "UserState.Null"); //null

	//对齐空间
	UE_DEFINE_GAMEPLAY_TAG(AlignSpace_Local, "AlignSpace.Local");
	UE_DEFINE_GAMEPLAY_TAG(AlignSpace_World, "AlignSpace.World");

	//TransformMode
	UE_DEFINE_GAMEPLAY_TAG(TransformMode_Translation, "TransformMode.Translation");
	UE_DEFINE_GAMEPLAY_TAG(TransformMode_Rotation, "TransformMode.Rotation");
	UE_DEFINE_GAMEPLAY_TAG(TransformMode_Scale, "TransformMode.Scale");

	//约束轴
	UE_DEFINE_GAMEPLAY_TAG(ConstraintAxis_X, "ConstraintAxis.X");
	UE_DEFINE_GAMEPLAY_TAG(ConstraintAxis_Y, "ConstraintAxis.Y");
	UE_DEFINE_GAMEPLAY_TAG(ConstraintAxis_Z, "ConstraintAxis.Z");
	UE_DEFINE_GAMEPLAY_TAG(ConstraintAxis_XY, "ConstraintAxis.XY");
	UE_DEFINE_GAMEPLAY_TAG(ConstraintAxis_XZ, "ConstraintAxis.XZ");
	UE_DEFINE_GAMEPLAY_TAG(ConstraintAxis_YZ, "ConstraintAxis.YZ");
	UE_DEFINE_GAMEPLAY_TAG(ConstraintAxis_XYZ, "ConstraintAxis.XYZ");
	UE_DEFINE_GAMEPLAY_TAG(ConstraintAxis_Screen, "ConstraintAxis.Screen");

	UE_DEFINE_GAMEPLAY_TAG(PointerInteractionSource_World, "PointerInteractionSource.World");
	UE_DEFINE_GAMEPLAY_TAG(PointerInteractionSource_MouseCursor, "PointerInteractionSource.MouseCursor");
	UE_DEFINE_GAMEPLAY_TAG(PointerInteractionSource_ScreenCenter, "PointerInteractionSource.ScreenCenter");
	UE_DEFINE_GAMEPLAY_TAG(PointerInteractionSource_Custom, "PointerInteractionSource.Custom");
	UE_DEFINE_GAMEPLAY_TAG(PointerInteractionSource_Overlap, "PointerInteractionSource.Overlap");

	//编辑模式下子模块
	UE_DEFINE_GAMEPLAY_TAG(Edit_Mark, "Edit.Mark")
	UE_DEFINE_GAMEPLAY_TAG(Edit_Device, "Edit.Device")
	UE_DEFINE_GAMEPLAY_TAG(Edit_Pawn, "Edit.Pawn")
	UE_DEFINE_GAMEPLAY_TAG(Edit_Scene, "Edit.Scene")
	UE_DEFINE_GAMEPLAY_TAG(Edit_Asset, "Edit.Asset")

	UE_DEFINE_GAMEPLAY_TAG(FileType_Folder, "FileType.Folder")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Image, "FileType.Image")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Datasmith, "FileType.Datasmith")
	UE_DEFINE_GAMEPLAY_TAG(FileType_FBX, "FileType.FBX")
	
	UE_DEFINE_GAMEPLAY_TAG(MeshStatus_Move, "MeshStatus.Move")
	UE_DEFINE_GAMEPLAY_TAG(MeshStatus_Replace, "MeshStatus.Replace")
	
	UE_DEFINE_GAMEPLAY_TAG(FolderStatus_MeshModuleResource, "FolderStatus.MeshModuleResource")
	UE_DEFINE_GAMEPLAY_TAG(FolderStatus_Drag, "FolderStatus.Drag")
	UE_DEFINE_GAMEPLAY_TAG(FolderStatus_Function, "FolderStatus.Function")
	
	//UE_DEFINE_GAMEPLAY_TAG(FileType_Folder, "FileType.Folder")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Point, "FileType.Point")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Line, "FileType.Line")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Surface, "FileType.Surface")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Tile, "FileType.Tile")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Map, "FileType.Map")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Mesh, "FileType.Mesh")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Circle, "FileType.Circle")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Arrow, "FileType.Arrow")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Decal, "FileType.Decal")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Projection, "FileType.Projection")
	UE_DEFINE_GAMEPLAY_TAG(FileType_WidgetProjection, "FileType.WidgetProjection")
	UE_DEFINE_GAMEPLAY_TAG(FileType_PointCloud, "FileType.PointCloud")
	UE_DEFINE_GAMEPLAY_TAG(FileType_Plant, "FileType.Plant")

	UE_DEFINE_GAMEPLAY_TAG(VisitStatus_Local, "VisitStatus.Local")
	UE_DEFINE_GAMEPLAY_TAG(VisitStatus_Network, "VisitStatus.Network")

	UE_DEFINE_GAMEPLAY_TAG(FunctionType_Default, "FunctionType.Default")
	UE_DEFINE_GAMEPLAY_TAG(FunctionType_Point, "FunctionType.Point")
	UE_DEFINE_GAMEPLAY_TAG(FunctionType_Line, "FunctionType.Line")
	UE_DEFINE_GAMEPLAY_TAG(FunctionType_Surface, "FunctionType.Surface")
	UE_DEFINE_GAMEPLAY_TAG(FunctionType_Circle, "FunctionType.Circle")
	UE_DEFINE_GAMEPLAY_TAG(FunctionType_Arrow, "FunctionType.Arrow")
	UE_DEFINE_GAMEPLAY_TAG(FunctionType_Decal, "FunctionType.Decal")
	UE_DEFINE_GAMEPLAY_TAG(FunctionType_Projection, "FunctionType.Projection")
	UE_DEFINE_GAMEPLAY_TAG(FunctionType_WidgetProjection, "FunctionType.WidgetProjection")
	UE_DEFINE_GAMEPLAY_TAG(FunctionType_PointCloud, "FunctionType.PointCloud")
	UE_DEFINE_GAMEPLAY_TAG(FunctionType_Plant, "FunctionType.Plant")

	UE_DEFINE_GAMEPLAY_TAG(EditStatus_Current, "EditStatus.Current")
	UE_DEFINE_GAMEPLAY_TAG(EditStatus_Previous, "EditStatus.Previous")

	UE_DEFINE_GAMEPLAY_TAG(EditType_Normal, "EditType.Normal")
	UE_DEFINE_GAMEPLAY_TAG(EditType_Normal_Dynamic, "EditType.Normal.Dynamic")
	UE_DEFINE_GAMEPLAY_TAG(EditType_Check, "EditType.Check")
	UE_DEFINE_GAMEPLAY_TAG(EditType_Select, "EditType.Select")
	UE_DEFINE_GAMEPLAY_TAG(EditType_Option, "EditType.Option")
	
	UE_DEFINE_GAMEPLAY_TAG(EMessageType_Normal, "MessageType.Normal")
	UE_DEFINE_GAMEPLAY_TAG(EMessageType_Waning, "MessageType.Waning")
	UE_DEFINE_GAMEPLAY_TAG(EMessageType_Error, "MessageType.Error")
	
	UE_DEFINE_GAMEPLAY_TAG(E3DUI_Type_DeviceInfo, "3DUI.Type.DeviceInfo")
	UE_DEFINE_GAMEPLAY_TAG(E3DUI_Type_DeviceInfoSelect, "3DUI.Type.DeviceInfoSelect")
	UE_DEFINE_GAMEPLAY_TAG(E3DUI_Type_TrainStationMarking, "3DUI.Type.TrainStationMarking")

}

CommonToolTag::CommonToolTag()
{
	
}

CommonToolTag::~CommonToolTag()
{
	
}
