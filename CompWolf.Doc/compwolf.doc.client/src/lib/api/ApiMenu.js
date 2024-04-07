import MenuTree from "@/lib/MenuTree";
import { Reference } from "./CodeComponents";

export default function ApiMenu(props) {
	return <MenuTree data={props.data} fields={["projects", "headers", "entities"]} name={(name, path) => {
		return <Reference path={path}>{name}</Reference>
	}} />
}