import FunctionViewer from "@/lib/api/FunctionViewer";
import { getEntity } from "@/lib/api/Entity";

export default async function ApiPage({ params }) {
    const owner = await getEntity(params.project, params.header, params.entity)
    const target = params.subentity

    var data = undefined;
    if (target == params.entity) data = owner.constructor
    else if (target in owner.dataMembers) data = owner.dataMembers[target]
    else owner.methodGroups.find((i) => {
        i.items.find((j) => {
            if (j.name == target) data = j;
            return (data != undefined);
        })
        return (data != undefined);
    })

    data = {
        ...data,
        project: owner.project,
        header: owner.header,
        owner: owner.name,
        namespace: owner.namespace,
        name: target,
    }

    return <FunctionViewer data={data} />
}
