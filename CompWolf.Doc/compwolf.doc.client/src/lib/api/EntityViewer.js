import BaseEntityViewer from "./BaseEntityViewer";
import ClassViewer from "./ClassViewer";
import FunctionViewer from "./FunctionViewer";

export default function EntityViewer(props) {
	const data = props.data;
	switch (data?.type) {
		case `class`: return <ClassViewer data={data} />
		case `function`: return <FunctionViewer data={data}/>
		default: return <BaseEntityViewer />
    }
}