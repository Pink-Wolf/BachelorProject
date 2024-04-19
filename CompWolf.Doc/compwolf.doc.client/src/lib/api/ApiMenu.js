import MenuTree from "@/lib/MenuTree";
import { Reference } from "../CodeComponents";

export default function ApiMenu({ data }) {
	return <MenuTree data={data} fields={["projects", "headers", "entities"]} name={(name, path) => {
		return <Reference path={path}>{name}</Reference>
	}} />
}